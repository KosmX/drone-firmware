//
// Created by kosmx on 10/22/23.
//

#ifndef STUFF_TELEMETRY_H
#define STUFF_TELEMETRY_H

#ifdef __cplusplus

#include "os/os.h"
#include "usart.h"
#include "os/mutex_wapper.h"

#include <string>
#include <memory>

/**
 * Lot a specific message
 * @param str instance, it will be freed
 */
void log(std::unique_ptr<std::string>&& str);

void log(std::string& str);

void log(std::string&& str);

namespace tlm {

    class ITelemetry {
    public:
        static ITelemetry* INSTANCE;
        virtual void log(std::unique_ptr<std::string>&& str) = 0; // log a message, it will free the message
    };

    class DummyTelemetry : public ITelemetry {
        void log(std::unique_ptr<std::string> &&str) override;
    };

    class UartTelemetry : public ITelemetry {
    private:
        static UartTelemetry* INSTANCE;
        QueueHandle_t queueHandle;
        SemaphoreHandle_t uartSemaphore;
        UART_HandleTypeDef& uart;

        [[noreturn]] void run();

    public:

        explicit UartTelemetry(UART_HandleTypeDef& huart);

        void log(std::unique_ptr<std::string> &&str) override;

    };

}

#endif
#endif //STUFF_TELEMETRY_H
