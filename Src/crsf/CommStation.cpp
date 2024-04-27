//
// Created by kosmx on 4/5/24.
//

#include "CommStation.h"
#include "telemetry.h"
#include "crc.h"
#include "config/Config.h"
#include "config/ConfigResponseBuilder.h"
#include "Packets.h"


CommStation* CommStation::INSTANCE = nullptr;
os::AtomicData<CommStation::DShotData> CommStation::motorData{};

namespace crsf {


    constexpr const osThreadAttr_t thread_attr = {
            .name = "telemetry",
            .stack_size = 128 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };

    void pass(const RxPacket&) {}

    ELRSController::ELRSController(os::uart_dma &uart): uart(uart),
    handlers{
            {
                    CRSF_FRAMETYPE_DEVICE_PING,     [&](const RxPacket &rec) {
                sendPacket([&](TxPacket &p) {
                    p.defaultRoute();
                    p.writeString("FlightController");

                    auto data = "ELRS";
                    p.writeBytes(reinterpret_cast<const uint8_t *>(data), 4);

                    uint8_t hwVer[] = {0, 0, 0, 1};
                    p.writeBytes(hwVer, 4);

                    uint8_t swVer[] = {0, 0, 0, 0};
                    p.writeBytes(swVer, 4);
                    p.writeByte(cfg::cfg.size());
                    p.writeByte(0);
                });
            }
            },
            {
                    CRSF_FRAMETYPE_PARAMETER_READ,  [&](const RxPacket &p) {
                uint8_t filedIdx = p.readByte();
                uint8_t chunkIdx = p.readByte();

                sendPacket([&](TxPacket &tx) {
                    // this will properly configure and write
                    cfg::ConfigResponseBuilder::create(tx, filedIdx, chunkIdx);
                });
            }
            },
            {
                    CRSF_FRAMETYPE_PARAMETER_WRITE, [&](const cfg::ConfigUpdatePacket &p) {
                p.writeConfig();
            }
            },
            {
                CRSF_FRAMETYPE_LINK_STATISTICS, pass
            },
            {
                CRSF_FRAMETYPE_RC_CHANNELS_PACKED, [&](const ChannelData &p) {
                    CommStation::motorData = p.readChannels();
                }
            }
    }
    {
        handleThread = osThreadNew([](void* p) {
            auto* pThis = reinterpret_cast<ELRSController*>(p);
            pThis->runThread();

        }, this, &thread_attr);
    }

    /**
     * Thread function
     */
    [[noreturn]] void ELRSController::runThread() {

        const TickType_t frequency = pdMS_TO_TICKS(10);
        TickType_t lastWakeTime;


        while (true) {

            checkForData();
            // delay before doing everything again
            vTaskDelayUntil(&lastWakeTime, frequency);
        }
    }


    int ELRSController::checkForData() {
        int packets = 0;

        while (uart.available() >= 2) {
            auto len = uart.peek(1);
            if (uart.available() < CRSF_FRAME_SIZE(len)) break; // not enough data yet

            RxPacket conf{uart.read(CRSF_FRAME_SIZE(len))};
            // TODO verify CRC

            auto crc = GenericCRC8::PolyD5.calc((conf.pData + 2), conf.getLength() + 1);
            if (crc != conf.getCrc()) {
                tlm::ITelemetry::INSTANCE->log(std::make_unique<std::string>("CRC doesn't match for packet"));
            }

            try {
                handlers.at(static_cast<crsf_frame_type_e>(conf.getPacketId()))(conf);
            } catch (std::out_of_range& e) {
                tlm::ITelemetry::INSTANCE->log(std::make_unique<std::string>("Unknown packet ID: " + std::to_string(conf.getPacketId())));
            }
            packets++;
        }
        if (counter++ >= 50) {
            counter = 0;
            sendPacket([](TxPacket& p) {
                HeartBeat::write(p);
            });
        }
        return packets;
    }

    void ELRSController::sendPacket(std::function<void(TxPacket &)> f) {
        buf.configBack([&](msgData& pData) {
            TxPacket p{pData.buf};
            f(p);
            if (p.getWriteIndex() != 0) { // if packet was constructed with writeIdx, generate CRC
                p.calcCRCAndSetPacketLength();
            }
        });
    }

    void ELRSController::initComm() {
         // ?
    }

} // crsf