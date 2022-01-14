/*
 * Copyright (c) 2015-2021 University of Antwerp, Aloxy NV.
 *
 * This file is part of Sub-IoT.
 * See https://github.com/Sub-IoT/Sub-IoT-Stack for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This examples pushes sensor data to gateway(s) by manually constructing an ALP command with a file read result action
// (unsolicited message). The D7 session is configured to request ACKs. All received ACKs are printed.
// Temperature data is used as a sensor value, when a HTS221 is available, otherwise value 0 is used.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hwleds.h"
#include "hwsystem.h"
#include "hwlcd.h"

#include "scheduler.h"
#include "timer.h"
#include "debug.h"
#include "d7ap_fs.h"
#include "log.h"

#include "d7ap.h"
#include "alp_layer.h"
#include "dae.h"

#include "platform.h"
#include "led.h"

#ifdef USE_SHT21
#include "sht21_driver.h"
#include "hwi2c.h"
#endif

#define SENSOR_FILE_ID 0x42
#define SENSOR_FILE_SIZE 5
#define SENSOR_INTERVAL_SEC TIMER_TICKS_PER_SEC * 30

#define SENSOR_FILE_SENSOR_DATA_OFFSET 0
#define SENSOR_FILE_LED_OFFSET 4

#ifdef USE_SHT21
static i2c_handle_t *sht21_handle;
#endif

// this example pushes the temperature in a file together with the led status.
// The led status can be modified by sending a (dormant session) write file to the device claiming the led is on.

// Define the D7 interface configuration used for sending the ALP command on

static alp_interface_config_d7ap_t itf_config = (alp_interface_config_d7ap_t) {
  .itf_id = ALP_ITF_ID_D7ASP,
  .d7ap_session_config = {
    .qos = {
      .qos_resp_mode = SESSION_RESP_MODE_PREFERRED,
      .qos_retry_mode = SESSION_RETRY_MODE_NO
    },
    .dormant_timeout = 0,
    .addressee = {
      .ctrl = {
        .nls_method = AES_NONE,
        .id_type = ID_TYPE_NOID,
      },
      .access_class = 0x01,
      .id = {0}
    }
  }
};

static uint8_t led_status = 0;

void execute_sensor_measurement()
{
  log_print_string("executing sensor measurement");
  // first get the sensor readings ...
  int16_t sensor_data[2] = {0, 0};
  uint8_t data[5];

#if defined USE_SHT21
  i2c_acquire(sht21_handle);
  sht21_read_temperature(sht21_handle, &sensor_data[0]);
  sht21_read_humidity(sht21_handle, &sensor_data[1]);
  i2c_release(sht21_handle);
#endif

  sensor_data[0] = __builtin_bswap16(sensor_data[0]); // convert to big endian before transmission
  sensor_data[1] = __builtin_bswap16(sensor_data[1]); // convert to big endian before transmission

  memcpy(&data[SENSOR_FILE_SENSOR_DATA_OFFSET], (uint8_t *)&sensor_data[0], sizeof(sensor_data));

  data[SENSOR_FILE_LED_OFFSET] = led_status;

  // Generate ALP command.
  // We will be sending a return file data action, without a preceding file read request.
  // This is an unsolicited message, where we push the sensor data to the gateway(s).

  // alloc command. This will be freed when the command completes
  alp_command_t *command = alp_layer_command_alloc(false, false);

  // forward to the D7 interface
  alp_append_forward_action(command, (alp_interface_config_t *)&itf_config, sizeof(itf_config));

  // add the return file data action
  alp_append_return_file_data_action(command, SENSOR_FILE_ID, 0, SENSOR_FILE_SIZE, data);

  // and finally execute this
  alp_layer_process(command);
}

void on_alp_command_completed_cb(uint8_t tag_id, bool success)
{
  if (success)
  {
    log_print_string("Command (%i) completed successfully", tag_id);
    led_flash_green();
  }
  else
  {
    log_print_string("Command failed, no ack received");
    led_flash_red();
  }

  // reschedule sensor measurement
  if (!timer_is_task_scheduled(&execute_sensor_measurement))
    timer_post_task_delay(&execute_sensor_measurement, SENSOR_INTERVAL_SEC);
}

void on_alp_command_result_cb(alp_command_t *alp_command, alp_interface_status_t *origin_itf_status)
{
  if (origin_itf_status && (origin_itf_status->itf_id == ALP_ITF_ID_D7ASP) && (origin_itf_status->len > 0))
  {
    d7ap_session_result_t *d7_result = ((d7ap_session_result_t *)origin_itf_status->itf_status);
    log_print_string("recv response @ %i dB link budget from:", d7_result->rx_level);
    log_print_data(d7_result->addressee.id, d7ap_addressee_id_length(d7_result->addressee.ctrl.id_type));
  }
  log_print_string("response payload:");
  log_print_data(alp_command->alp_command, fifo_get_size(&alp_command->alp_command_fifo));
  fifo_skip(&alp_command->alp_command_fifo, fifo_get_size(&alp_command->alp_command_fifo));
}

static void file_modified_callback(uint8_t file_id)
{
  log_print_string("file modified callback");
  uint32_t length = 1;
  
  d7ap_fs_read_file(SENSOR_FILE_ID, SENSOR_FILE_LED_OFFSET, &led_status, &length, ROOT_AUTH);
  if (led_status)
    led_on(LED_BLUE);
  else
    led_off(LED_BLUE);

  timer_cancel_task(&execute_sensor_measurement);
  sched_cancel_task(&execute_sensor_measurement);
  // give some time to handle the current dialog first
  assert(timer_post_task_delay(&execute_sensor_measurement, 100) == SUCCESS);
}

static alp_init_args_t alp_init_args = {
  .alp_command_completed_cb = &on_alp_command_completed_cb,
  .alp_command_result_cb = &on_alp_command_result_cb
};

static const d7ap_fs_file_header_t sensor_file_header = {
  .allocated_length = SENSOR_FILE_SIZE,
  .length = SENSOR_FILE_SIZE,
  .file_permissions = (file_permission_t) {
    .guest_read = true, 
    .guest_write = true, 
    .user_read = true, 
    .user_write = true
  },
  .file_properties.storage_class = FS_STORAGE_PERMANENT
};

void bootstrap()
{

  log_print_string("Device booted\n");

  alp_layer_init(&alp_init_args, false);

  sched_register_task(&execute_sensor_measurement);

  d7ap_fs_init_file(SENSOR_FILE_ID, &sensor_file_header, NULL);
  // register a callback for when the sensor file is modified
  d7ap_fs_register_file_modified_callback(SENSOR_FILE_ID, &file_modified_callback);
  // already trigger this callback to ensure we're already in the right state
  file_modified_callback(SENSOR_FILE_ID);

  // activate low power listening
  d7ap_fs_write_dll_conf_active_access_class(0x11);

  led_init();

#if defined USE_SHT21
  sht21_handle = i2c_init(0, 0, 100000, true);
#endif
}
