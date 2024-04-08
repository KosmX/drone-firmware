//
// Created by kosmx on 4/5/24.
//

#ifndef DRONE_FW_COMMSTATION_H
#define DRONE_FW_COMMSTATION_H

#include <os/uart_dma.h>
#include "cmsis_os2.h"
#include "os/mutex_wapper.h"
#include <tl/RingBuffer.h>

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
    private:
        os::uart_dma& uartInstance;


        osThreadId_t handleThread;
        osThreadId_t sendThread;

        [[noreturn]] void runThread();

        tl::RingBuffer<uint8_t[64]> buf{8};

    public:

        struct MessageEntry {
            uint8_t data[64] = {0};
        };
        explicit ELRSController(os::uart_dma& uart);

        bool isControllerPresent() const override;




    };

} // crsf
#endif //DRONE_FW_COMMSTATION_H
