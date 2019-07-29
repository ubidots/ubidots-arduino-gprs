/*
    Copyright (c) 2017, Ubidots.

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

    Original Maker: Mateo Velez - Metavix
    Modified and maintained by: Jose Garcia https://github.com/jotathebest
                                Maria Carlina Hernandez https://github.com/mariacarlinahernandez 
*/

#ifndef __UbidotsArduinoGPRS_H_
#define __UbidotsArduinoGPRS_H_

#include <stdint.h>
#include <Arduino.h>
#include <Stream.h>
#include <stdio.h>  

namespace {
    const char * SERVER = "translate.ubidots.com";
    const char * TIME_SERVER = "pool.ntp.org";
    const char * PORT = "9012";
    const char * USER_AGENT = "GPRS";
    const char * VERSION = "3.0.0";
    const float ERROR_VALUE = -3.4028235E+8;
    const uint8_t MAX_VALUES = 5;
    const uint8_t DEFAULT_BUFFER_SIZE = 64;

}

typedef struct Value {
    char  *varName;
    char  *contextOne;
    float varValue;
    unsigned long timestamp_val;
} Value;

class Ubidots {

 public:
    explicit Ubidots(char* token, char* server = SERVER);
    bool init(Stream &port);
    bool sendAll();
    bool sendAll(unsigned long timestamp_global);
    float getValueWithDevice(char* device_label, char* variable_label);
    void add(char *variable_label, double value);
    void add(char *variable_label, double value, char *ctext);
    void add(char *variable_label, double value, char *ctext, unsigned long timestamp);
    void flushInput();
    void setApn(char* apn = "", char* user = "", char* pwd = "");
    void setDeviceName(char* deviceName);
    void setDeviceLabel(char* deviceLabel);
    void setDebug(bool debug);
    unsigned long ntpUnixTime();


 private:
    bool _debug = true;
    bool isTimedOut(uint32_t ts) { return (long)(millis() - ts) >= 0; }
    bool manageData(char* allData);
    char* _apn;
    char* _apn_user;
    char* _apn_pwd;
    char* _device_label;
    char* _device_name;
    char* _server;
    char* _token;
    char buffer[DEFAULT_BUFFER_SIZE];
    char* readData(uint16_t timeout);
    uint8_t _currentValue;
    void powerUpOrDown();    
    Stream *client;
    Value * val;
};

#endif
