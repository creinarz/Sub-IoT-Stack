/**
 ******************************************************************************
 * @file      : sht21_driver.c
 * @brief     : Source code for the SHT21 driver
 ******************************************************************************
 * Author: Anastasis Vagenas -> Contact: anasvag29@gmail.com
 *
 * This Library has routines in order to communicate with SHT21 sensor either via HAL
 * or Direct access (using LL or Direct access to registers).
 * For the I2C Driver user can choose either HAL or our own implementation in the i2c_driver.h
 *
 * ------- ROUTINES -------
 * I2C Routines:
 * - i2c_SHT2x_write_reg() -> Writes a SHT21 register
 * - i2c_SHT2x_write_cmd() -> Writes a write command to the I2C
 * - i2c_SHT2x_read_reg() -> Reads from the SHT21 register
 *
 * Temperature/Humidity Data Routines:
 * - SHT2x_CalcTemp() -> Reads the temperature data
 * - SHT2x_CalcHumid() -> Reads the humidity data
 * - SHT2x_CalcAll() -> Reads all of the above data
 *
 * Configuration Routines:
 * - SHT2x_write_user_reg()/SHT2x_read_user_reg -> Writes/Reads to the User register
 * - SHT2x_reset() -> Perform a reset command
 * - SHT2x_init() -> Initializes the sensor with default configuration
 * - SHT2x_peripheral_config() -> Initializes the I2C peripheral and the GPIOs used
 *
 * Extra Routines:
 * - SHT2x_CheckCrc() -> Checks the CRC of the received data (if it is enabled)
 *
 * ------- MAIN DATA STRUCTURES -------
 * SHT21_t -> Holds measurements for humidity/temperature
 *
 * ------- USAGE -------
 * In order to simply use the driver, set:
 * 1.An I2C handle based on the header file naming used
 * 2.Set the Defines for the GPIOs used (if they are a different board)
 *
 * Then call:
 * 1.SHT2x_peripheral_config() -> Initialize the peripherals
 * 2.SHT2x_init() -> Initialize the SHT21 with the basic configuration
 * 3.Use the Data Routines (i.e SHT2x_CalcAll()) to read data from the MPU6050.
 *
 **/

/* Includes ------------------------------------------------------------------*/
#include "sht21_driver.h"

#define SHT21_I2C_ADDRESS 0x80

#define SHT_READ_TEMP_NOHOLD 0xF3
#define SHT_READ_TEMP_HOLD 0xE3
#define SHT_READ_HUM_NOHOLD 0xF5
#define SHT_READ_HUM_HOLD 0xE5
#define SHT_READ_USER_REG 0xE7
#define SHT_WRITE_USER_REG 0xE6
#define SHT_SOFT_RESET 0xFE

#define SHT_TEMP_FLAG 0x00
#define SHT_RH_FLAG 0x02

#define POLYNOMIAL 0x131 // P(x)=x^8+x^5+x^4+1 = 100110001

#define READ_TIMEOUT_TCKS 50

extern uint8_t sensor_io_write(void* handle, uint8_t WriteAddr, uint8_t* pBuffer, uint16_t nBytesToWrite);
extern uint8_t sensor_io_read(void* handle, uint8_t ReadAddr, uint8_t* pBuffer, uint16_t nBytesToRead);

static int read_timeout = 0;

/* -- SHT2x_CheckCrc() --
 * Input: Register address, Register Write data , Number of data
 * Return: None
 * Description:
 *
 * Internal function of the SHT2x driver, that uses I2C to read the specified data
 * from to the specified register address of the SHT2x.
 * Can be used to read conversion data (temperature - humidity) or
 *
 * */
static int8_t sht21_check_crc(uint8_t data[], uint8_t n, uint8_t chck)
{
    uint8_t crc = 0;
    uint8_t byteCtr;
    for (byteCtr = 0; byteCtr < n; ++byteCtr) {
        crc ^= (data[byteCtr]);
        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return (crc == chck) ? 0 : -1;
}

static int8_t sht21_read_value(void* handle, uint8_t cmd, uint16_t* data)
{
    uint8_t buf[3];
    buf[0] = cmd;

    //  just writes to the I2C
    sensor_io_write(handle, SHT21_I2C_ADDRESS >> 1, &buf[0], 1);

    uint32_t tickstart = 0;
    tickstart = HAL_GetTick();

    while (1) {
        HAL_Delay(5);
        // stop after successful reading
        if (sensor_io_read(handle, SHT21_I2C_ADDRESS >> 1, &buf[0], 3) == 0) {
            break;
        }
        if ((HAL_GetTick() - tickstart) >= 85) {
            return -1;
        }
    }
    if (sht21_check_crc(buf, 2, buf[2]) >= 0) {
        *data = (buf[0] << 8 | buf[1]);
        return 0;
    } else {
        return -1;
    }
}

sht21_error_t sht21_read_temperature(void* handle, int16_t* temperature)
{
    uint16_t value;
    if (sht21_read_value(handle, SHT_READ_TEMP_NOHOLD, &value) >= 0) {
        if ((value & 0x02) == SHT_TEMP_FLAG) {
            value &= 0xfffc;
            // Formula is T = -46.85 + 175.72 * value / 2^16
            *temperature = (((-4685 * 16384) + 4393 * (int32_t)value + 8192) / 16384);
            return SHT21_OK;
        }
    }
    // something went wrong
    return SHT21_ERROR;
}

sht21_error_t sht21_read_humidity(void* handle, int16_t* humidity)
{
    uint16_t value;

    if (sht21_read_value(handle, SHT_READ_HUM_NOHOLD, &value) >= 0) {
        if ((value & 0x02) == SHT_RH_FLAG) {
            value &= 0xfffc;
            // Formula is rH = -6 + 125 * value / 2^16
            *humidity = (((-12 * 32768) + 125 * (uint32_t)value + 16384) / 32768);
            return SHT21_OK;
        }
    }
    return SHT21_ERROR;
}
