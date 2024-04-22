//
// Created by kosmx on 4/5/24.
//

#ifndef DRONE_FW_COMMSTATION_H
#define DRONE_FW_COMMSTATION_H

#include <os/uart_dma.h>
#include "cmsis_os2.h"
#include "os/mutex_wapper.h"
#include <tl/RingBuffer.h>
#include "CRSF_listener.h"
#include <map>

/**
 * Communication interface for controller
 * I make it an interface for later upgrade to controller-less mode
 */
class CommStation {
public:
    static CommStation* INSTANCE;

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
        };

    private:
        os::uart_dma& uart;


        osThreadId_t handleThread{};
        osThreadId_t sendThread{};

        [[noreturn]] void runThread();

        tl::RingBuffer<msgData> buf{8};


        // RxPacket handling will be a procedural switch case.
        // OOP sucks for many reasons
        typedef std::function<void(const RxPacket& config)> HandlerFunction;

        const std::map<crsf_frame_type_e, HandlerFunction> handlers;

        /**
         * BLOCKING
         * init ELRS connection
         * return when connection is active
         */
        void initComm();

        /**
         * Handle received data, including validation and processing
         * @param pData volatile pointer to data. Its content is safe for processing, but is going to be invalid after handlePacket returns
         */
        void handlePacket(os::RingBufferEntryPtr pData);

        /**
         * Check for new data, and if there is any, handle it
         * @param uart handler
         * @return amount of packets handled
         */
         int checkForData();
    public:

        struct MessageEntry {
            uint8_t data[64] = {0};
        };
        explicit ELRSController(os::uart_dma& uart);

        bool isControllerPresent() const override;

        void sendPacket(std::function<void(TxPacket&)> f);




    };

} // crsf
#endif //DRONE_FW_COMMSTATION_H
