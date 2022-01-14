# DASH7 GETTING STARTED
## DASH7 source code for Miromico FMLR STM SX1272 modules
- Copy or clone the source code from https://github.com/creinarz/Sub-IoT-Stack.git
- Unzip `fmlr_apps` to some directory and modfify `DAPP_EXTRA_APPS_DIR` accordingly. I just place them in your/path/to/Sub-IoT-Stack untill I find a better way to organize off tree applications. 
- Compile with the following CMake configuration arguments:
  - `DPLATFORM=FMLR_72_X_STL0Z`
  - `DAPP_EXTRA_APPS_DIR=${PROJECT_SOURCE_DIR}/../fmlr_apps`
  - `DAPP_FMLR_GATEWAY=y`
  - `DAPP_FMLR_SENSOR_PUSH=y`
  - `DAPP_FMLR_SENSOR_PULL=y`
  - `DAPP_FMLR_SENSOR_PUSH_LED=y`

  I place these options in VSCode settings.json

## You will also need pyd7a
This is a collection of python modules to support the DASH7 protocol. Unfortunately pyd7a and all example applications are python 2.7 but I think there is an update to python 3 in the making.
- Copy or clone from https://github.com/Sub-IoT/pyd7a
- You will find the installation instructions in README.md


## RUNNING THE EXAMPLES
You will need one FMLR module for the gateway and one or more modules for the sensors. Messages from sensors to the gateway are forwarded over the serial port to pyd7a application examples.
Messages to a sensor are sent over the serial port to the gateway and then forwarded to the sensors via DASH7.
On the gateway the blue LED should flash once per second just so we know it's running.
Green LED should flash when data is received from a sensor.

- Flash one of `fmlr_sensor_push`, `fmlr_sensor_pull` or `fmlr_sensor_push_led` to one+ FMLR modules. These apps are modified to use the RGB LED and SHT21 sensor on the FMLR Evaluation Kit STM.
- Flash fmlr_gateway to another FMLR module.
- Connect the module with the gateway application to a PC via USB.
- Depending on which app you flashed, run one of the following applications on the PC:


#### `unsolicited_response_logger.py for fmlr_sensor_push`
Sends temperature and humidity to the gateway every 10 seconds using unsolicited response messages. Green LED flashes when the sensor receives an acknowledge from the gateway
Red LED flashes if no acknowledge is received within timeout

- cd to `your/path/to/pyd7a`
- Run `PYTHONPATH=. python -u examples/unsolicited_response_logger.py -d /dev/cu.usbserial-XXXXXXXX` on the PC.

#### `query_nodes.py` for `fmlr_sensor_pull`
Uses background scanning to wait for a temperature/humidity request from the gateway.
Green LED flashes when new sensor data is available. I have been told it's possible to go as low as 30uA with background scanning. Not for coin cell battery but an AA or A sized LiSoCl4 battery or similar should do just fine for a few years.
- cd to `your/path/to/pyd7a`
- Run `PYTHONPATH=. python -u examples/query_nodes.py -d /dev/cu.usbserial-XXXXXXXX` on the PC to query the nodes.


### `sensor_push_led.py` for `fmlr_senosr_push_led`
This is the most interesting application, it sends temperature and humidity using unsolicited response messages every 30 seconds and waits for a command from the gateway to turn the blue LED on or off using either background scanning or dormant sessions. The latter means that the LED's are only updated as a response to a message from the sensor.

Green LED flashes when the sensor receives an acknowledge from the gateway. Red LED flashes if no acknowledge is received within afew seconds.
Data is forwarded to an MQTT broker and can be seen there.

- Run MQTT broker on local host 
- cd to `your/path/to/pyd7a`
- Run  `PYTHONPATH=. python -u examples/sensor_push_led.py -d /dev/cu.usbserial-XXXXXXXX`
- Add option `-b hostname` if you are using a remote broker. 

To turn the blue LED on or off do:
- `mosquitto_pub -m "DON" -t home/led-cmd         // LED ON/OFF via dormant session.`
- `mosquitto_pub -m "XON" -t home/led-cmd		// LED ON/OFF with background scanning.`     
- Only `D` and `ON` are relevant. `OFF` or `X` can be anything.


## Modify channel class and other default parameters if needed
`d7ap_fs_data.c` contains default values and access profiles for the DASH7 stack. You will find it in `your/path/to/Sub-IoT-Stack/stack/fs`. You probably will want to modify `channel_band=ChannelBand.BAND_915` to `channel_band=ChannelBand.BAND_868` (line 66)
- Run `python2 -m cogapp -c -r your/path/to/Sub-IoT-Stack/stack/fs/d7ap_fs_data.c`
- Recompile all sources. 
These parameters can also be modified locally or remotely using in code ALP commands, but that's another story. 

There are other examples in `stack/apps` you might want to take a look at. I have not tested all of them but, be warned, some don't compile or don't work. `pyd7a/examples/engineering_mode.py` does work and is interesting for testing PER and other stuff at different frequencies and rates. You need to flash two boards with the gateway app, connect each to a PC and run `engineering_mode.py` on both, one with option `--mode PER_TX` and the other with option `--mode PER_RX`. Available options area easy to find in the source code.