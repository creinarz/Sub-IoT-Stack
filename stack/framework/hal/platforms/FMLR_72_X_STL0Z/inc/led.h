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

#ifndef __PLATFORM_LED_H_
#define __PLATFORM_LED_H_

#include "hwleds.h"
#include "timer.h"

#define FLASH_DURATION TIMER_TICKS_PER_SEC * 0.1

#define LED_RED   0
#define LED_GREEN 1
#define LED_BLUE  2
#define LED_BOARD 3

bool led_init(void);

void led_on(uint8_t led_nr);
void led_off(unsigned char led_nr);
void led_toggle(unsigned char led_nr);
void led_flash_green(void);
void led_flash_blue(void);
void led_flash_red(void);

#endif
