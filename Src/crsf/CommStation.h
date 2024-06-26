//
// Created by kosmx on 4/5/24.
//

#ifndef DRONE_FW_COMMSTATION_H
#define DRONE_FW_COMMSTATION_H

#include <os/uart_dma.h>
#include "os/Task.h"
#include "os/mutex_wapper.h"
#include <tl/RingBuffer.h>
#include "CRSF_listener.h"
#include "os/atomic_data.h"
#include "Packets.h"
#include <map>

/**
 * Communication interface for controller
 * I make it an interface for later upgrade to controller-less mode
 */
class CommStation {
public:
    static CommStation* INSTANCE;

    static os::AtomicData<crsf::Channels> controlData;

    [[nodiscard]] virtual bool isControllerPresent() const = 0;
};

namespace crsf {

/** ExpressLRS=CRSF station
 *
 */
    class ELRSController : public CommStation {
    public:
        struct msgData {
            uint8_t buf[64];

            size_t size();
        };

    private:
        os::uart_dma& uart;


        os::Task handleThread{"CRSF_RX", osPriorityAboveNormal, 128*8};
        os::Task sendThread{"CRSF_TX"};

        [[noreturn]] void rx();

        void clearRX();

        tl::RingBuffer<msgData> buf{16};


        // RxPacket handling will be a procedural switch case.
        // OOP sucks for many reasons
        typedef std::function<void(const RxPacket& config)> HandlerFunction;

        const std::map<crsf_frame_type_e, HandlerFunction> handlers;

        /**
         * Check for new data, and if there is any, handle it
         * @param uart handler
         * @return amount of packets handled
         */
         int checkForData();

         [[noreturn]] void tx();
    public:

        struct MessageEntry {
            uint8_t data[64] = {0};
        };
        explicit ELRSController(os::uart_dma& uart);

        bool isControllerPresent() const override;

        /**
         * BLOCKING
         * init ELRS connection
         * return when connection is active
         */
        void initComm();

        void sendPacket(std::function<void(TxPacket&)> f);




    };

} // crsf
#endif //DRONE_FW_COMMSTATION_H
