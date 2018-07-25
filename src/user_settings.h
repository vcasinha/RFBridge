#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H
    #include <Arduino.h>

    #undef RF_THROTTLE_PER_SECOND
    #define RF_THROTTLE_PER_SECOND 1

    #define CYCLE_DELAY 100

    unsigned int serial_monitor_baud = 115200;

    unsigned int emitter_pin = D6;
    unsigned int receiver_pin = D5;

    const char * mqtt_hostname = "192.168.0.200";
    const char * mqtt_username = "mqtt";
    const char * mqtt_password = "mqtt";

    const char * ssid = "SSID";
    const char * password = "PASSWORD";

#endif