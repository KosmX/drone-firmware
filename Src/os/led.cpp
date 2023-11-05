//
// Created by kosmx on 11/1/23.
//

#include "led.h"

os::led::led(GPIO_TypeDef* bus, uint16_t port): bus{bus}, port{port} {

}

bool os::led::getState() const {
    return HAL_GPIO_ReadPin(bus, port);
}

void os::led::setState(bool newState) {
    HAL_GPIO_WritePin(bus, port, newState ? GPIO_PinState::GPIO_PIN_SET : GPIO_PinState::GPIO_PIN_RESET);
}

void os::led::toggle() {
    HAL_GPIO_TogglePin(bus, port);
}
