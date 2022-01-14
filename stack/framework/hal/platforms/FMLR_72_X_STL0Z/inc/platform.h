/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#include "platform_defs.h"
#include "stm32_device.h"
#include "stm32_common_mcu.h"
#include "stm32_common_eeprom.h"

#include "fs.h"
#include "hwblockdevice.h"
#include "blockdevice_ram.h"

#ifndef PLATFORM_FMLR_72_X_STL0Z
    #error Mismatch between the configured platform and the actual platform. Expected FMLR_72_X_STL0Z to be defined
#endif

/********************
 * LED DEFINITIONS *
 *******************/

#define LED1 PIN(GPIO_PORTH, 0)
#define LED2 PIN(GPIO_PORTH, 1)
#define LED3 PIN(GPIO_PORTC, 1)
#define LED4 PIN(GPIO_PORTB, 8)

/**************************
 * USERBUTTON DEFINITIONS *
 *************************/

#define BUTTON0           PIN(GPIO_PORTA, 0)

#if defined(USE_SX127X) || defined(USE_NETDEV_DRIVER)
  // TODO tmp
  #define SX127x_SPI_INDEX  0
  #define SX127x_SPI_PIN_CS  PIN(GPIO_PORTA, 4)
  #define SX127x_SPI_BAUDRATE 8000000
  #define SX127x_DIO0_PIN PIN(GPIO_PORTA, 1)
  #define SX127x_DIO1_PIN PIN(GPIO_PORTA, 6)
  #ifdef PLATFORM_SX127X_USE_DIO3_PIN
    #define SX127x_DIO3_PIN PIN(GPIO_PORTC, 4)
  #endif
  #ifdef PLATFORM_SX127X_USE_RESET_PIN
    #define SX127x_RESET_PIN PIN(GPIO_PORTC, 2)
  #endif
  #ifdef PLATFORM_SX127X_USE_VCC_TXCO
	#define SX127x_VCC_TXCO PIN(GPIO_PORTB, 12)
  #endif
#endif

/** Platform BD drivers*/
extern blockdevice_t * const metadata_blockdevice;
extern blockdevice_t * const persistent_files_blockdevice;
extern blockdevice_t * const volatile_blockdevice;
#define PLATFORM_METADATA_BLOCKDEVICE metadata_blockdevice
#define PLATFORM_PERMANENT_BLOCKDEVICE persistent_files_blockdevice
#define PLATFORM_VOLATILE_BLOCKDEVICE volatile_blockdevice

#endif

