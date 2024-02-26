//
// Created by kosmx on 11/14/23.
//

#ifndef STUFF_OS_SPI_H
#define STUFF_OS_SPI_H
#ifdef __cplusplus

#include "os/os.h"
#include "os/gpin.h"
#include "spi.h"
#include "mutex_wapper.h"

namespace os {


    /**
     *
     */
    class spi {
    private:
        spi(SPI_HandleTypeDef *spi, gpin *defaultNss);

        SPI_HandleTypeDef* hspi;
        SemaphoreHandle_t readSemaphore;
        SemaphoreHandle_t writeSemaphore;
        Mutex opMutex;
        gpin* defaultNss;

        spi(spi&): hspi{}, readSemaphore{}, writeSemaphore{} {}
        spi& operator=(const spi&);
    public:
        static spi* getFor(SPI_HandleTypeDef* spi, gpin* defaultNss = nullptr);

        HAL_StatusTypeDef receive(uint8_t *readData, uint32_t len, gpin* nss = nullptr);

        HAL_StatusTypeDef transmit(const uint8_t *writeData, uint32_t len, gpin* nss = nullptr);

        HAL_StatusTypeDef memRead(uint8_t addr, uint8_t *readData, uint32_t len, gpin* nss = nullptr);

        /**
         * Not as efficient as transmit, but does not need to allocate any extra memory
         * @param addr
         * @param writeData
         * @param len
         * @param nss
         * @return
         */
        HAL_StatusTypeDef memWrite(uint8_t addr, const uint8_t *writeData, uint32_t len, gpin* nss = nullptr);



    };

} // os

#endif //__cplusplus
#endif //STUFF_OS_SPI_H
