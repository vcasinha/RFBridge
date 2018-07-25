# RFBridge
RF bridge to MQTT

The bidirectional bridge between the two protocols. Repeats on one medium any code received on the other medium.

In working state. Contributions are welcome.

## Concept
To provide interaction between RF and MQTT as transparently as possible.

## Hardware

Works on:

### ESP8266
* Wemos D1 R2
* Wemos D1 R2 mini

## Using

Clone this repository and adjust settings in platformio.ini

```c++
    unsigned int serial_monitor_baud = 9600;
    unsigned int emitter_pin = D6;
    unsigned int receiver_pin = D5;
    const char * mqtt_hostname = "192.168.0.200";
    const char * mqtt_username = "mqtt";
    const char * mqtt_password = "mqtt";
    const char * ssid = "SSID";
    const char * password = "PASSWORD";
```

Upload the project

When the board boots WiFi Manager will take care of the connection. If the board is not configured or not able to connect to the WiFi network, an AP (Access Point) is created with an SSID ESP123111 (different numbers) and a captive portal is automatically started (on IP address 192.168.4.1).
Join the AP and fill out the configuration form.

## Troubleshooting

### Subscribe MQTT topics

```
# mosquitto_sub -d -h [MQTT_HOST] -q 1 -t "home/switch/rf/#" -u MQTT_USER -P MQTT_PASSWORD -v | grep -v "Client mosqsub"
home/switch/rf/000000F00FFF TOGGLE
home/switch/rfxtended/000000000000010000010101|1|24|367 TOGGLE
```

### Serial port monitor

```
RF bridge to MQTT 1.0
*WM: Adding parameter
*WM: hostname
*WM: Adding parameter
*WM: username
*WM: Adding parameter
*WM: password
*WM:
*WM: AutoConnect
*WM: Connecting as wifi client...
*WM: Using last saved values, should be faster
*WM: Connection result:
*WM: 3
*WM: IP Address:
*WM: 192.168.0.12
Connected to MQTT at 192.168.0.200
Initialization complete.
```


## Libraries used
* [Arduino](http://arduino.cc/)
* [RC Switch](https://github.com/sui77/rc-switch)
* [WiFi Manager](https://github.com/tzapu/WiFiManager/)
* [Arduino Client for MQTT](https://github.com/knolleary/pubsubclient)
* [ESP8266 core for Arduino ](https://github.com/esp8266/Arduino)
* [ESP8266WebServer](https://github.com/esp8266/ESPWebServer)