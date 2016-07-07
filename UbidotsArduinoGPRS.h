/*
    Copyright (c) 2013, Ubidots.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Made by Mateo Velez - Metavix
*/



#ifndef __UbidotsArduinoGPRS_H_
#define __UbidotsArduinoGPRS_H_
#define DEBUG_UBIDOTS

#include <stdint.h>
#include <Arduino.h>
#include <Stream.h>


#define SERVER "translate.ubidots.com"
#define PORT "9010"
#define MAX_VALUES 5
#define USER_AGENT "GPRS"
#define VERSION "2.0"
#define DEFAULT_BUFFER_SIZE 64

typedef struct Value {
    char  *varName;
    char  *ctext;
    float varValue;
} Value;

class Ubidots {
 public:
    explicit Ubidots(char* token, char* server = SERVER);
    bool init(Stream &port);
    bool setApn(char* apn = "", char* user = "", char* pwd = "");
    float getValueWithDatasource(char* dsTag, char* idName);
    void setDataSourceName(char* dsName);
    void setDataSourceTag(char* dsTag);
    bool sendAll();
    void add(char *variableName, float value, char* context = NULL);
    void powerUpOrDown();
    void flushInput();

 private:
    char* readData(uint16_t timeout);
    bool isTimedOut(uint32_t ts) { return (long)(millis() - ts) >= 0; }
    char buffer[DEFAULT_BUFFER_SIZE];
    char* _dsTag;
    char* _server;
    char* _dsName;
    char* _token;
    uint8_t currentValue;
    Stream *client;
    Value * val;
};


#endif