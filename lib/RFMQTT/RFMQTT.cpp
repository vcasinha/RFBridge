#include <Arduino.h>
#include "RFMQTT.h"

static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

void RFMQTT::setup(PubSubClient client, int emitter_pin, int receiver_pin, int emitter_repeat)
{
    this->_emitterPin = emitter_pin;
    this->_receiverPin = receiver_pin;
    this->_emitterRepeat = emitter_repeat;

    this->_driver.enableTransmit(emitter_pin);
    this->_driver.setRepeatTransmit(emitter_repeat);
    this->_driver.enableReceive(receiver_pin);

    this->_client = client;
    client.subscribe("home/switch/rf/set/#");

    this->_ready = true;
}

void RFMQTT::loop()
{
    if(this-_ready == false)
    {
        Serial.printf("Library not initialized.");
        abort();
    }

    if (this->_driver.available())
    {
        char topic[100];
        const char * payload = "TOGGLE";
        unsigned long decimal_code = this->_driver.getReceivedValue();
        unsigned int protocol = this->_driver.getReceivedProtocol();
        unsigned int bit_length = this->_driver.getReceivedBitlength();
        unsigned int received_delay = this->_driver.getReceivedDelay();
        const char * binary_code = dec2binWzerofill(decimal_code, bit_length);
        const char * tristate_code = bin2tristate(binary_code);

        Serial.printf("Received RF code: %s\n", tristate_code);

        this->_driver.resetAvailable();
        switch(this->_format)
        {
            case RFMQTT_TRISTATE:
                sprintf(topic, "home/switch/rf/%s", tristate_code);
                break;
            case RFMQTT_BINARY:
                sprintf(topic, "home/switch/rf/%s", binary_code);
                break;
            case RFMQTT_RAW:
                char buffer[20];
                ltoa(decimal_code, buffer, 16);
                sprintf(topic, "home/switch/rf/%s", buffer);
                break;
        }

        unsigned long time_now = millis();
        unsigned long time_delta = time_now - timeSinceMQTT;
        if(strcmp(tristate_code, _emittedMQTTCode) == 0 && time_delta < 1000)
        {
            Serial.printf(" Ignoring, probably a repetition. (%ld)\n", time_delta);
            return;
        }

        Serial.printf("    TD(%ld) Compare (%s vs %s)\n", time_delta, _emittedMQTTCode, tristate_code);

        
        sprintf(topic, "home/switch/rf/%s", tristate_code);
        this->_client.publish(topic, "TOGGLE");

        Serial.printf("   MQTT Topic: %s Payload: %s\n", topic, payload);

        sprintf(topic, "home/switch/rfxtended/%s|%d|%d|%d", binary_code, protocol, bit_length, received_delay);
        this->_client.publish(topic, payload);
        strcpy(_emittedMQTTCode, tristate_code);
        timeSinceMQTT = time_now;
    }
    //To use on MQTT callback
    //mySwitch.send(data, valueBITS);
}

//Test topic home/switch/rf/FFFF00F00FFF|1|24|367
void RFMQTT::callbackMQTT(char* topic, byte* payload, unsigned int length)
{
    //topic[length] = '\0';
    String topic_string = String(topic);
    Serial.printf("RF Bridge topic %s\n", topic_string.c_str());

    if(topic_string.startsWith("home/switch/rf/set/"))
    {
        //Serial.printf("RF Bridge code %s\n", topic_string.c_str());

        int i0 = topic_string.indexOf("set/");
        String code = topic_string.substring(i0 + 4);
        Serial.printf("   RF Code: %s\n", code.c_str());
        switch(this->_format)
        {
            case RFMQTT_TRISTATE:
                this->_driver.sendTriState(code.c_str());
                break;
            case RFMQTT_BINARY:
                this->_driver.send(code.c_str());
                break;
        }

        strcpy(_emittedRFCode, code.c_str());
    }
}

static const char* bin2tristate(const char* bin) 
{
    static char returnValue[50];
    int pos = 0;
    int pos2 = 0;
    while (bin[pos]!='\0' && bin[pos+1]!='\0') 
    {
        if (bin[pos]=='0' && bin[pos+1]=='0') 
        {
            returnValue[pos2] = '0';
        } 
        else if (bin[pos]=='1' && bin[pos+1]=='1') 
        {
            returnValue[pos2] = '1';
        } 
        else if (bin[pos]=='0' && bin[pos+1]=='1') 
        {
            returnValue[pos2] = 'F';
        } 
        else 
        {
            return "not applicable";
        }
        pos = pos+2;
        pos2++;
    }
    returnValue[pos2] = '\0';
    return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) 
{
    static char bin[64]; 
    unsigned int i=0;

    while (Dec > 0) {
        bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
        Dec = Dec >> 1;
    }

    for (unsigned int j = 0; j< bitLength; j++) 
    {
        if (j >= bitLength - i) 
        {
            bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
        } 
        else 
        {
            bin[j] = '0';
        }
    }
    bin[bitLength] = '\0';

    return bin;
}
