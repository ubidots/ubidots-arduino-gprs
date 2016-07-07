/*
    SODAQ library to using mDot bee. Some function are based on GPRSbee
    library of Kees Bakker
    Copyright (C) 2016  Mateo Velez - Metavix for Ubidots Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __Ubidots_Arduino_GPRS_H_
#define __Ubidots_Arduino_GPRS_H_
#define DEBUG_UBIDOTS

#include <stdint.h>
#include <Arduino.h>
#include <Stream.h>


#define SERVER "translate.ubidots.com"
#define PORT "9010"
#define MAX_VALUES 5
#define USER_AGENT "SODAQGPRSbee"
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