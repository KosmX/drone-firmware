//
// Created by kosmx on 11/14/23.
//

#include "bmi.h"
#include <utility>
#include "telemetry.h"

namespace drv {
    bmi::bmi(os::spi &spi, os::gpin &nGyro, os::gpin &nAccel) :
            dev{},
            syncCfg{},
            spi{spi},
            nGyro{nGyro},
            nAccel{nAccel} {

        dev.intf = BMI08_SPI_INTF;
        dev.intf_ptr_gyro = new std::pair<bmi*, bool>(this, false);
        dev.intf_ptr_accel = new std::pair<bmi*, bool>(this, true);

        dev.delay_us = [](uint32_t period, void *_this) {
            vTaskDelay(pdMS_TO_TICKS(period)); // no active waiting
        };

        dev.write = [](uint8_t reg_addr, const uint8_t *writeData, uint32_t len, void *pthis) -> BMI08_INTF_RET_TYPE {
            auto *pair = reinterpret_cast<std::pair<bmi*, bool>*>(pthis);
            auto* _this = pair->first;

            _this->spi.memWrite(reg_addr, writeData, len, &(pair->second ? _this->nAccel : _this->nGyro));

            return BMI08_INTF_RET_SUCCESS;
        };

        dev.read = [](uint8_t reg_addr, uint8_t *readData, uint32_t len, void *pthis) -> BMI08_INTF_RET_TYPE {
            auto *pair = reinterpret_cast<std::pair<bmi*, bool>*>(pthis);
            auto* _this = pair->first;

            _this->spi.memRead(reg_addr, readData, len, &(pair->second ? _this->nAccel : _this->nGyro));

            return BMI08_INTF_RET_SUCCESS;
        };

        // init


        if (bmi088_mma_init(&dev) != BMI08_OK || bmi08g_init(&dev) != BMI08_OK) {
            Error_Handler();
        }

        auto rslt = bmi08a_soft_reset(&dev);



        if (rslt == BMI08_OK)
        {
            /* Set accel power mode */
            dev.accel_cfg.power = BMI08_ACCEL_PM_ACTIVE;
            rslt = bmi08a_set_power_mode(&dev);
        }

        if (rslt == BMI08_OK)
        {
            dev.gyro_cfg.power = BMI08_GYRO_PM_NORMAL;
            rslt = bmi08g_set_power_mode(&dev);
        }

        if ((dev.accel_cfg.power != BMI08_ACCEL_PM_ACTIVE) || (dev.gyro_cfg.power != BMI08_GYRO_PM_NORMAL))
        {
            ("Accel/gyro sensor in suspend mode\nUse in active/normal mode !!");
            exit(EXIT_FAILURE);
        }

        log("Uploading BMI08 data synchronization feature config !\n");

        /*API uploads the bmi08 config file onto the device*/
        if (rslt == BMI08_OK)
        {
            rslt = bmi08a_load_config_file(&dev);

            /* Wait for 150ms to enable the data synchronization --delay taken care inside the function */
            if (rslt == BMI08_OK)
            {
                dev.accel_cfg.range = BMI088_MM_ACCEL_RANGE_24G;

                /* Assign gyro range setting*/
                dev.gyro_cfg.range = BMI08_GYRO_RANGE_2000_DPS;

                /*! Mode (0 = off, 1 = 400Hz, 2 = 1kHz, 3 = 2kHz) */
                syncCfg.mode = BMI08_ACCEL_DATA_SYNC_MODE_400HZ;
                rslt = bmi088_mma_configure_data_synchronization(syncCfg, &dev);
            }
        }

        if (rslt == BMI08_OK)
        {
            log("BMI08 data synchronization feature configured !\n");
        }
        else
        {
            log("BMI08 data synchronization feature configuration failure!\n");
            Error_Handler();
        }
    }



} // drv