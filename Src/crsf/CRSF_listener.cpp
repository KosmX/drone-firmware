//
// Created by kosmx on 11/18/23.
//

#include "CRSF_listener.h"
#include "usart.h"


namespace crsf {

    // This is a ring-buffer
    char rx_buf[256];
    unsigned int read = 0;
    unsigned int write = 0;

    void init() {
        HAL_UART_Receive_DMA()
    }
}