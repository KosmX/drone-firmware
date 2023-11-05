//
// Created by kosmx on 11/1/23.
//

#ifndef STUFF_LED_H
#define STUFF_LED_H

#include "gpio.h"

namespace os {


    class led {
    private:
        GPIO_TypeDef* bus;
        uint16_t port;


    public:
        led(GPIO_TypeDef* bus, uint16_t port);

        [[nodiscard]] bool getState() const;
        void setState(bool newState);

        void toggle();

    };


}

#endif //STUFF_LED_H
