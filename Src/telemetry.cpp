//
// Created by kosmx on 10/22/23.
//

#include "telemetry.h"

#include "cmsis_os2.h"
#include "mutex_wapper.h"


// static

tlm::ITelemetry* tlm::ITelemetry::INSTANCE = nullptr;
tlm::UartTelemetry* tlm::UartTelemetry::INSTANCE = nullptr;

//


osThreadId_t telemetryTaskHandle;
const osThreadAttr_t telemetry_attr = {
        .name = "telemetry",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityRealtime,
};

tlm::UartTelemetry::UartTelemetry(UART_HandleTypeDef& huart): queueHandle{xQueueCreate(8, sizeof(void*))}, uart{huart} {

    uartSemaphore = xSemaphoreCreateBinary();
    UartTelemetry::INSTANCE = this;


    telemetryTaskHandle = osThreadNew([](void* pthis) {
        auto* _this = reinterpret_cast<tlm::UartTelemetry*>(pthis);
        _this->run();
    }, this, &telemetry_attr);


    HAL_UART_RegisterCallback(&huart, HAL_UART_CallbackIDTypeDef::HAL_UART_TX_COMPLETE_CB_ID, [](UART_HandleTypeDef *huart) {
        auto newTask = pdFALSE;
        xSemaphoreGiveFromISR(UartTelemetry::INSTANCE->uartSemaphore, &newTask);
    });

    xSemaphoreGive(uartSemaphore);
}

[[noreturn]] void tlm::UartTelemetry::run() {
    std::string *str = nullptr;
    while (true) {

        xQueueReceive(queueHandle, &str, portMAX_DELAY); // async waiting for a new message, if there is any, we'll start transmitting


        if (str != nullptr) {
            xSemaphoreTake(uartSemaphore, portMAX_DELAY); // take the UART for transmit, interrupt will set it free once transmit is complete
            HAL_UART_Transmit_DMA(&uart, reinterpret_cast<const uint8_t *>(str->c_str()), str->size());

            // UART transmit has been started, but we'll need our data back

            xSemaphoreTake(uartSemaphore, portMAX_DELAY); // it will block until uart is working, then we can free our message (string)
            delete str;
            xSemaphoreGive(uartSemaphore); // anyway, we aren't transmitting again, the semaphore is free for now.

        }
    }
}

void tlm::UartTelemetry::log(std::unique_ptr<std::string> &&str) {
    auto str_ptr = std::move(str);

    auto ptr = str_ptr.release();
    if (xQueueSend(queueHandle, &ptr, portMAX_DELAY) != pdTRUE) {
        delete ptr; // queue wasn't successful, deleting message
    }
}

void log(std::unique_ptr<std::string>&& msg) {
    tlm::ITelemetry::INSTANCE->log(std::move(msg)); // just forward ownership
}

void log(std::string& msg) {
    log(std::make_unique<std::string>(msg)); // probably a copy ctor
}

void log(std::string&& msg) {
    log(std::make_unique<std::string>(msg)); // rvalue, move ctor
}

void tlm::DummyTelemetry::log(std::unique_ptr<std::string> &&str) {
    std::unique_ptr<std::string> s = std::move(str); // placeholder to make sure unique_ptr will be deleted (this is still not Rust)
}
