//
// Created by kosmx on 11/14/23.
//

#include "bmi.h"
#include <utility>
#include "telemetry.h"
#include "bmi08x.h"

namespace drv {

    static void enable_bmi08_data_synchronization_interrupt(struct bmi08_dev *bmi08dev)
    {
        struct bmi08_int_cfg int_config{};

        /* Set accel interrupt pin configuration */
        /* Configure host data ready interrupt */
        int_config.accel_int_config_1.int_channel = BMI08_INT_CHANNEL_2;
        int_config.accel_int_config_1.int_type = BMI08_ACCEL_SYNC_INPUT;
        int_config.accel_int_config_1.int_pin_cfg.output_mode = BMI08_INT_MODE_PUSH_PULL;
        int_config.accel_int_config_1.int_pin_cfg.lvl = BMI08_INT_ACTIVE_HIGH;
        int_config.accel_int_config_1.int_pin_cfg.enable_int_pin = BMI08_ENABLE;

        /* Configure Accel syncronization input interrupt pin */
        int_config.accel_int_config_2.int_channel = BMI08_INT_CHANNEL_1;
        int_config.accel_int_config_2.int_type = BMI08_ACCEL_INT_SYNC_DATA_RDY;
        int_config.accel_int_config_2.int_pin_cfg.output_mode = BMI08_INT_MODE_PUSH_PULL;
        int_config.accel_int_config_2.int_pin_cfg.lvl = BMI08_INT_ACTIVE_HIGH;
        int_config.accel_int_config_2.int_pin_cfg.enable_int_pin = BMI08_ENABLE;

        /* Set gyro interrupt pin configuration */
        int_config.gyro_int_config_1.int_channel = BMI08_INT_CHANNEL_4;
        int_config.gyro_int_config_1.int_type = BMI08_GYRO_INT_DATA_RDY;
        int_config.gyro_int_config_1.int_pin_cfg.enable_int_pin = BMI08_ENABLE;
        int_config.gyro_int_config_1.int_pin_cfg.lvl = BMI08_INT_ACTIVE_HIGH;
        int_config.gyro_int_config_1.int_pin_cfg.output_mode = BMI08_INT_MODE_PUSH_PULL;

        int_config.gyro_int_config_2.int_channel = BMI08_INT_CHANNEL_3;
        int_config.gyro_int_config_2.int_type = BMI08_GYRO_INT_DATA_RDY;
        int_config.gyro_int_config_2.int_pin_cfg.enable_int_pin = BMI08_ENABLE;
        int_config.gyro_int_config_2.int_pin_cfg.lvl = BMI08_INT_ACTIVE_HIGH;
        int_config.gyro_int_config_2.int_pin_cfg.output_mode = BMI08_INT_MODE_PUSH_PULL;

        auto rslt = bmi08a_set_data_sync_int_config(&int_config, bmi08dev);

        if (rslt != BMI08_OK)
        {
            log("BMI08 data synchronization enable interrupt configuration failure!\n");
            //Error_Handler();
        }
    }


    bmi::bmi(os::spi &spi, os::gpin nGyro, os::gpin nAccel) :
            dev{},
            syncCfg{},
            spi{spi},
            nGyro{nGyro},
            nAccel{nAccel} {

        dev.intf = BMI08_SPI_INTF;
        dev.intf_ptr_gyro = new std::pair<bmi*, bool>(this, false);
        dev.intf_ptr_accel = new std::pair<bmi*, bool>(this, true);
        dev.read_write_len = 46; // magic constant from a bosch example, maybe it works. This whole feature is undocumented...

        dev.variant = BMI088_VARIANT;

        //dev.config_file_ptr = bmi08a_load_config_file()


        dev.delay_us = [](uint32_t period, void *_this) {
            vTaskDelay(pdMS_TO_TICKS(((period + 999)/1000))); // no active waiting
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


        if (bmi08xa_init(&dev) != BMI08_OK || bmi08g_init(&dev) != BMI08_OK) {
            Error_Handler();
        }

        auto rslt = bmi08a_soft_reset(&dev);

        rslt |= bmi08a_load_config_file(&dev);

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
            log("Accel/gyro sensor in suspend mode\nUse in active/normal mode !!");
            Error_Handler();
        }

        log("Uploading BMI08 data synchronization feature config !\n");

        /*API uploads the bmi08 config file onto the device*/
        if (rslt == BMI08_OK) {

            /* Wait for 150ms to enable the data synchronization --delay taken care inside the function */
            dev.accel_cfg.range = BMI088_MM_ACCEL_RANGE_24G;

            /* Assign gyro range setting*/
            dev.gyro_cfg.range = BMI08_GYRO_RANGE_2000_DPS;

            /*! Mode (0 = off, 1 = 400Hz, 2 = 1kHz, 3 = 2kHz) */
            syncCfg.mode = BMI08_ACCEL_DATA_SYNC_MODE_1000HZ;
            rslt = bmi08xa_configure_data_synchronization(syncCfg, &dev);

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


        enable_bmi08_data_synchronization_interrupt(&dev);
    }

    std::pair<struct bmi08_sensor_data, struct bmi08_sensor_data> bmi::getData() {
        struct bmi08_sensor_data accel{};
        struct bmi08_sensor_data gyro{};
        bmi08a_get_data(&accel, &dev);
        bmi08g_get_data(&gyro, &dev);
        //bmi08a_get_synchronized_data(&accel, &gyro, &dev); // well, idk why but it doesn't work
        return {accel, gyro};
    }


} // drv