/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2017 University of Antwerp
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

#ifndef __PORTS_H_
#define __PORTS_H_

#include "stm32_device.h"
#include "stm32_common_mcu.h"
#include "platform_defs.h"
#include "hwgpio.h"


#define SPI_COUNT 1
static const spi_port_t spi_ports[SPI_COUNT] = {
  {
    .spi = SPI2,
    .alternate_miso = GPIO_AF0_SPI2,
    .miso_pin = PIN(GPIO_PORTB, 14),
    .alternate_mosi = GPIO_AF0_SPI2,
    .mosi_pin = PIN(GPIO_PORTB, 15),
    .alternate_sck = GPIO_AF5_SPI2,
    .sck_pin = PIN(GPIO_PORTB, 10),
    .alternate = GPIO_AF0_SPI2,
  }
};

#define UART_COUNT 2
static const uart_port_t uart_ports[UART_COUNT] = {
  {
    // USART2, connected to VCOM of debugger USB connection
    .tx = PIN(GPIO_PORTA, 2),
    .rx = PIN(GPIO_PORTA, 3),
    .alternate = GPIO_AF4_USART2,
    .uart = USART2,
    .irq = USART2_IRQn
  },
  {
    // USART1, exposed on CN3 header: TX=PA9, RX=PA10
    .tx = PIN(GPIO_PORTA, 9),
    .rx = PIN(GPIO_PORTA, 10),
    .alternate = GPIO_AF4_USART1,
    .uart = USART1,
    .irq = USART1_IRQn
  }
};

#define I2C_COUNT 1
static const i2c_port_t i2c_ports[I2C_COUNT] = {
  {
    .i2c = I2C1,
    .scl_pin = PIN(GPIO_PORTB, 6),
    .alternate_scl = GPIO_AF1_I2C1,
    .sda_pin = PIN(GPIO_PORTB, 9),
    .alternate_sda = GPIO_AF4_I2C1,
    .alternate = GPIO_AF4_I2C1,
  }
};

static pin_id_t debug_pins[PLATFORM_NUM_DEBUGPINS] = {
  PIN(GPIO_PORTA, 13), 
  PIN(GPIO_PORTA, 14), 
};

#endif
