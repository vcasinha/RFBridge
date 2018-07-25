#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "RFMQTT.h"

#include "user_settings.h"

#define VERSION "1.0"
WiFiClient wifiClient;
WiFiServer server(80);
WiFiManager wifiManager;
WiFiManagerParameter custom_mqtt_hostname("hostname", "mqtt hostname", mqtt_hostname, 40);
WiFiManagerParameter custom_mqtt_username("username", "mqtt username", mqtt_username, 20);
WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 20);
PubSubClient client(wifiClient);
RFMQTT RFbridge;

String hostname = String("ESP8266-");

void callback(char * topic, unsigned char * payload, unsigned int length) 
{
    //Serial.printf("(CALLBACK) %s\n", topic);
    RFbridge.callbackMQTT(topic, payload, length);
}

//generate unique name from MAC addr
String format_mac(const uint8_t* mac)
{
    String result;

    for (int i = 0; i < 6; ++i) 
    {
        result += String(mac[i], 16);
    }

    return result;
}

void connect() 
{
    if(WiFi.status() == WL_CONNECTED)
    {
        // Loop until we're reconnected to the MQTT server
        while (client.connected() == false) 
        {            
            if (client.connect((char*) hostname.c_str(), custom_mqtt_username.getValue(), custom_mqtt_password.getValue()))
            {
                Serial.printf("Connected to MQTT at %s\n", mqtt_hostname);
            }
            else
            {
                Serial.printf("Could not connect to MQTT at %s\n", mqtt_hostname);
                abort();
            }
        }
    }
}

void update_hostname(void)
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    hostname += format_mac(mac);
}

void setup() 
{
    ///
    update_hostname();
    
    // put your setup code here, to run once:
    Serial.begin(serial_monitor_baud);
    delay(100);
    //start wifi subsystem
    Serial.printf("\n\nRF bridge to MQTT unit %s version %s\n\n", hostname.c_str(), VERSION);
    Serial.println("Initialize WiFi...");
    WiFi.setAutoReconnect(true);

    //WiFi Manager setup
    wifiManager.addParameter(&custom_mqtt_hostname);
    wifiManager.addParameter(&custom_mqtt_username);
    wifiManager.addParameter(&custom_mqtt_password);

    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect();
    //Serial.setDebugOutput(true);
    //WiFi.begin(ssid, password);
    //WiFi.printDiag(Serial);
    delay(100);
    Serial.println("Initialize MQTT Client...");
    client.setServer(custom_mqtt_hostname.getValue(), 1883);
    client.setCallback(callback);
    connect();

    delay(100);

    Serial.println("Initialize RF Bridge...");
    RFbridge.setup(client, emitter_pin, receiver_pin, 15);
    Serial.println("Unit ready");
}

void loop() 
{
    if (!client.connected())
    {
        connect();
    }

    client.loop();

    RFbridge.loop();
    delay(CYCLE_DELAY);
}
