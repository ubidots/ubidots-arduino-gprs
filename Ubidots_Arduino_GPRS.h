#ifndef __Ubidots_Arduino_GPRS_H_
#define __Ubidots_Arduino_GPRS_H_
#define DEBUG_UBIDOTS

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#define USER_AGENT  "Ubidots/v1 ArduinoGPRS/1.0"
#define BAUDRATE 19200

typedef struct Value {
  char  *id;
  char  *context_1;
  char  *context_2;
  float value_id;
} Value;

class Ubidots{
    private:
        SoftwareSerial _client = SoftwareSerial(7, 8);      
        bool httpInit();
        char* _token;
        bool httpTerm();
        char* readData(uint16_t timeout);
        void flushInput();
        uint8_t maxValues;
        uint8_t currentValue;
        Value * val;        

    public:
        Ubidots(char* token);
        void powerUpOrDown();
        bool setApn(char* apn, char* user, char* pwd);
        bool saveValue(double value, char* id);
        float getValue(char* id);
        void add(char *variable_id, double value, char *context1, char *context2);
        bool sendAll();        
};
#endif