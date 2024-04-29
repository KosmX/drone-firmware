//
// Created by kosmx on 11/14/23.
//

#include "os_spi.h"
#include <algorithm>
#include <vector>

namespace os {

    std::vector<std::pair<SPI_HandleTypeDef *, spi*>> spi_instances{};


    spi::spi(SPI_HandleTypeDef *spi, gpin *defaultNss) :
            hspi{spi},
            readSemaphore{xSemaphoreCreateBinary()},
            writeSemaphore{xSemaphoreCreateBinary()},
            opMutex{},
            defaultNss{defaultNss} {

        hspi->RxCpltCallback = [](SPI_HandleTypeDef* _spi){
            auto* _this = spi::getFor(_spi);

            auto p = pdTRUE;
            xSemaphoreGiveFromISR(_this->readSemaphore, &p);
            portYIELD_FROM_ISR(p)
        };

        hspi->TxCpltCallback = [](SPI_HandleTypeDef* _spi){
            auto* _this = spi::getFor(_spi);

            auto p = pdTRUE;
            xSemaphoreGiveFromISR(_this->writeSemaphore, &p);
            portYIELD_FROM_ISR(p)
        };

        hspi->RxCpltCallback = [](SPI_HandleTypeDef* _spi){
            auto* _this = spi::getFor(_spi);

            auto p = pdTRUE;
            xSemaphoreGiveFromISR(_this->readSemaphore, &p);
            xSemaphoreGiveFromISR(_this->writeSemaphore, &p);
            portYIELD_FROM_ISR(p)
        };

    }

    spi *spi::getFor(SPI_HandleTypeDef *hspi, gpin *defaultNss) {
        auto entry = std::find_if(spi_instances.cbegin(), spi_instances.cend(), [=](auto& it) -> bool{
            return it.first == hspi;
        });
        if (entry != spi_instances.cend()) {
            return entry->second;
        }
        spi_instances.push_back(std::pair(hspi, new spi(hspi, defaultNss)));
        return (spi_instances[spi_instances.size() - 1]).second;
    }

    HAL_StatusTypeDef spi::receive(uint8_t *readData, uint32_t len, gpin *nss) {
        auto l = opMutex.lock();

        nss = nss != nullptr ? nss : defaultNss;

        if (nss != nullptr) {
            nss->setState(false);
        }

        HAL_SPI_Receive_DMA(hspi, readData, len);

        xSemaphoreTake(readSemaphore, portMAX_DELAY);

        if (nss != nullptr) {
            nss->setState(true);
        }

        return HAL_OK;
    }

    HAL_StatusTypeDef spi::transmit(const uint8_t *writeData, uint32_t len, gpin *nss) {
        auto l = opMutex.lock();

        nss = nss != nullptr ? nss : defaultNss;

        if (nss != nullptr) {
            nss->setState(false);
        }

        HAL_SPI_Transmit_DMA(hspi, const_cast<uint8_t*>(writeData), len);

        xSemaphoreTake(writeSemaphore, portMAX_DELAY);

        if (nss != nullptr) {
            nss->setState(true);
        }

        return HAL_OK;
    }

    HAL_StatusTypeDef spi::memRead(uint8_t addr, uint8_t *readData, uint32_t len, gpin *nss) {
        auto l = opMutex.lock();

        nss = nss != nullptr ? nss : defaultNss;

        if (nss != nullptr) {
            nss->setState(false);
        }

        // transmitting 1 byte, at 6 MHz, task switching might be slower
        HAL_SPI_Transmit(hspi, &addr, 1, 10U);
        HAL_SPI_Receive_DMA(hspi, readData, len);

        xSemaphoreTake(readSemaphore, portMAX_DELAY);

        if (nss != nullptr) {
            nss->setState(true);
        }

        return HAL_OK;
    }

    HAL_StatusTypeDef spi::memWrite(uint8_t addr, const uint8_t *writeData, uint32_t len, gpin *nss) {
        auto l = opMutex.lock();

        nss = nss != nullptr ? nss : defaultNss;

        if (nss != nullptr) {
            nss->setState(false);
        }

        // transmitting 1 byte, at 6 MHz, task switching might be slower
        HAL_SPI_Transmit(hspi, &addr, 1, 10U);
        HAL_SPI_Transmit_DMA(hspi, const_cast<uint8_t*>(writeData), len);

        xSemaphoreTake(writeSemaphore, portMAX_DELAY);

        if (nss != nullptr) {
            nss->setState(true);
        }

        return HAL_OK;
    }

} // os