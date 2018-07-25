#include <Arduino.h>
#include <PubSubClient.h>
#include <RCSwitch.h>

#define RFMQTT_RAW 0
#define RFMQTT_BINARY 1
#define RFMQTT_TRISTATE 2

class RFMQTT 
{
    public:
        unsigned int _format = RFMQTT_TRISTATE;
        RCSwitch _driver;
        PubSubClient _client;
        long _counter = 0;
        char _emittedRFCode[30];
        char _emittedMQTTCode[30];
        unsigned long timeSinceMQTT;
        unsigned int _emitterPin;
        unsigned int _receiverPin;
        unsigned int _emitterRepeat = 15;
        bool _ready = false;

        void setFormat(unsigned int format);
        void setup(PubSubClient client, int emitter_pin, int receiver_pin, int emitter_repeat);
        void callbackMQTT(char * topic, unsigned char * payload, unsigned int length);
        void loop();
};