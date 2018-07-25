#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H
    #include <Arduino.h>

    unsigned int serial_monitor_baud = 9600;
    unsigned int emitter_pin = D6;
    unsigned int receiver_pin = D5;
    const char * mqtt_hostname = "192.168.0.200";
    const char * mqtt_username = "mqtt";
    const char * mqtt_password = "mqtt";
    const char * ssid = "SSID";
    const char * password = "PASSWORD";

#endif