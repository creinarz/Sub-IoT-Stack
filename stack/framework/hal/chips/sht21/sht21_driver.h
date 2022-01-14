/**
  ******************************************************************************
  * @file   : sht21_driver.h
  * @brief  : Header for main.c file.
  * This file contains the common defines of the application.
  ******************************************************************************
  * Author: Anastasis Vagenas -> Contact: anasvag29@gmail.com
  *
  *
  ******************************************************************************
  */
#ifndef SHT21_DRIV_H //Define to prevent recursive inclusion//
#define SHT21_DRIV_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#include <string.h>
#include <stdio.h>

// SHT21 Return Codes //
typedef enum { SHT21_OK = (uint8_t)0, SHT21_ERROR = !SHT21_OK } sht21_error_t;

// User register settings //
#define USR_REG_MASK		0xc7
#define ENABLE_HEATER		0x04
#define DISABLE_HEATER	0x00
#define DISABLE_OTP		  0x02
#define RH_12_TEM_14		0x00
#define RH_8_TEM_12		  0x01
#define RH_10_TEM_13		0x80
#define RH_11_TEM_11		0x81

/* Public function prototypes -----------------------------------------------*/
sht21_error_t sht21_read_temperature(void *handle, int16_t *temperature);
sht21_error_t sht21_read_humidity(void *handle, int16_t *humidity);
#endif	