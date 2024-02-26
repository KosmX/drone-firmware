//
// Created by kosmx on 11/1/23.
//

#ifndef STUFF_GPIN_H
#define STUFF_GPIN_H

#include "gpio.h"

namespace os {


    /**
     * Wrapper for a single GPIO pin
     */
    class gpin {
    private:
        GPIO_TypeDef* bus;
        uint16_t port;


    public:
        gpin(GPIO_TypeDef* bus, uint16_t port);

        [[nodiscard]] bool getState() const;
        void setState(bool newState);

        void toggle();

    };


}

#endif //STUFF_GPIN_H
