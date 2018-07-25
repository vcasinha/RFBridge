#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "RFMQTT.h"

#include "user_settings.h"

#define VERSION "1.0"

WiFiManager wifiManager;
WiFiManagerParameter custom_mqtt_hostname("hostname", "mqtt hostname", mqtt_hostname, 40);
WiFiManagerParameter custom_mqtt_username("username", "mqtt username", mqtt_username, 20);
WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 20);
RFMQTT RFbridge;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiServer server(80);

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
            // Generate client name based on MAC address and last 8 bits of microsecond counter
            String clientName;
            clientName += "esp8266-";
            uint8_t mac[6];
            WiFi.macAddress(mac);
            clientName += format_mac(mac);

            if (client.connect((char*) clientName.c_str(), custom_mqtt_username.getValue(), custom_mqtt_password.getValue()))
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

void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    //start wifi subsystem
    Serial.printf("\n\nRF bridge to MQTT %s\n", VERSION); 
    delay(100);

    WiFi.setAutoReconnect(true);
    wifiManager.addParameter(&custom_mqtt_hostname);
    wifiManager.addParameter(&custom_mqtt_username);
    wifiManager.addParameter(&custom_mqtt_password);
    wifiManager.autoConnect();
    delay(100);

    client.setServer(custom_mqtt_hostname.getValue(), 1883);
    client.setCallback(callback);
    //Serial.setDebugOutput(true);
    //WiFi.begin(ssid, password);
    delay(100);

    //WiFi.printDiag(Serial);

    connect();

    delay(1000);
    RFbridge.setup(client, D6, D5, 15);
    Serial.println("Initialization complete.");
}

void loop() 
{
    if (!client.connected())
    {
        connect();
    }

    client.loop();

    RFbridge.loop();
    delay(100);
}
