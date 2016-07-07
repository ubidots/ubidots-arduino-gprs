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



#include <avr/pgmspace.h>
#ifdef ARDUINO_ARCH_AVR
#include <avr/wdt.h>
#else
#define wdt_reset()
#endif
#include "Ubidots_Arduino_GPRS.h"


/**
 * Constructor.
 */
Ubidots::Ubidots(char* token, char* server) {
    _server = server;
    _token = token;
    _dsName = NULL;
    client = 0;
    _dsTag = "GPRS";
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
}
bool Ubidots::init(Stream &port) {
    client = &port;
    powerUpOrDown();
    return true;
}

/** 
 * This function is to power up or down GPRS Shield
 */
void Ubidots::powerUpOrDown(){
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);
  readData(4000);
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////        GPRS Functions       /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Ubidots::setDataSourceName(char* dsName) {
    _dsName = dsName;
}
void Ubidots::setDataSourceTag(char* dsTag) {
    _dsTag = dsTag;
}

float Ubidots::getValueWithDatasource(char* dsTag, char* idName) {
    float num;
    int i = 0;
    String allData;
    allData = USER_AGENT;
    allData += "/";
    allData += VERSION;
    allData += "|LV|";
    allData += _token;
    allData += "|";
    allData += dsTag;
    allData += ":";
    allData += idName;
    allData += "|end";
    String response;
    uint8_t bodyPosinit;
    client->println(F("AT+CIPMUX=0"));
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPMUX"));
#endif
        return false;
    }
    client->print(F("AT+CIPSTART=\"TCP\",\""));
    client->print(_server);
    client->print(F("\",\""));
    client->print(PORT);
    client->println(F("\""));
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSTART"));
#endif
        return false;
    }
    client->print(F("AT+CIPSEND="));
    client->println(allData.length());
    if(strstr(readData(4000),">")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSEND"));
#endif
        return false;
    }
    client->write(allData.c_str());
    response = String(readData(4000));
    Serial.println(response);
    client->println(F("AT+CIPCLOSE"));
    if(strstr(readData(4000),"CLOSE OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending data"));
#endif
        return false;
    }
    client->println(F("AT+CIPSHUT"));
    if(strstr(readData(4000),"SHUT OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSHUT"));
#endif
        return false;
    }
    bodyPosinit = 3 + response.indexOf("OK|");
    response = response.substring(bodyPosinit);
    num = response.toFloat();
    return num;
}

bool Ubidots::setApn(char* apn, char* user, char* pwd) {
    char message[9][50];
    int i = 0;
    client->println(F("AT"));
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT"));
#endif
        return false;
    }
    client->println(F("AT+CSQ"));
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CSQ"));
#endif
        return false;
    }
    client->println(F("AT+CGATT?"));    
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CGATT"));
#endif
        return false;
    }
    client->println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));        
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR CONTYPE"));
#endif
        return false;
    }
    client->print(F("AT+SAPBR=3,1,\"APN\",\""));
    client->print(apn);
    client->println(F("\""));   
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR APN"));
#endif
        return false;
    }   
    client->print(F("AT+SAPBR=3,1,\"USER\",\""));
    client->print(user);
    client->println(F("\""));       
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR USER"));
#endif
        return false;
    }   
    client->print(F("AT+SAPBR=3,1,\"PWD\",\""));
    client->print(pwd);
    client->println("\"");              
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR PASSWORD"));
#endif
        return false;
    }
    client->println(F("AT+SAPBR=1,1"));     
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=1,1 Connection ip"));
#endif
        return false;
    }
    client->println(F("AT+SAPBR=2,1"));     
    if(strstr(readData(4000),"+SAPBR:")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=2,1 no IP to show"));
#endif
        return false;
    }
    return true;
}

void Ubidots::add(char *variableName, float value, char *context) {
    (val+currentValue)->varName = variableName;
    (val+currentValue)->ctext = context;
    (val+currentValue)->varValue = value;
    currentValue++;
    if (currentValue > MAX_VALUES) {
        currentValue = MAX_VALUES;
    }
}

/**
 * Send all data of all variables that you saved
 * @reutrn true upon success, false upon error.
 */

bool Ubidots::sendAll() {
    int i;
    String all;
    String str;
    all = USER_AGENT;
    all += "/";
    all += VERSION;
    all += "|POST|";
    all += _token;
    all += "|";
    all += _dsTag;
    if (_dsName != NULL) {
        all += ":";
        all += _dsName;
    }
    all += "=>";
    for (i = 0; i < currentValue; ) {
        str = String(((val + i)->varValue), 2);
        all += String((val + i)->varName);
        all += ":";
        all += str;
        if ((val + i)->ctext != NULL) {
            all += "$";
            all += String((val + i)->ctext);
        }
        all += ",";
        i++;
    }
    all += "|end";
    Serial.println(all.c_str());
    client->println(F("AT+CIPMUX=0"));
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPMUX"));
#endif
        return false;
    }
    client->print(F("AT+CIPSTART=\"TCP\",\""));
    client->print(_server);
    client->print(F("\",\""));
    client->print(PORT);
    client->println(F("\""));
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSTART"));
#endif
        return false;
    }
    client->print(F("AT+CIPSEND="));
    client->println(all.length());
    if(strstr(readData(4000),">")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSEND"));
#endif
        return false;
    }
    client->write(all.c_str());
    if(strstr(readData(4000),"SEND OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending variables"));
#endif
        return false;
    }
    client->println(F("AT+CIPCLOSE"));
    if(strstr(readData(4000),"CLOSE OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPCLOSE"));
#endif
        return false;
    }
    client->println(F("AT+CIPSHUT"));
    if(strstr(readData(4000),"SHUT OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSHUT"));
#endif
        return false;
    }
    currentValue = 0;
    return true;
}

/** 
 * This function is to read the data from GPRS pins. This function is from Adafruit_FONA library
 * @arg timeout, time to delay until the data is transmited
 * @return replybuffer the data of the GPRS
 */
char* Ubidots::readData(uint16_t timeout){
  uint16_t replyidx = 0;
  char replybuffer[500];
  while (timeout--) {
    if (replyidx >= 500) {
      break;
    }
    while(client->available()) {
      char c =  client->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;
      }
      replybuffer[replyidx] = c;
      replyidx++;
    }

    if (timeout == 0) {
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = '\0';  // null term
#ifdef DEBUG_UBIDOTS
  Serial.println("Response of GPRS:");
  Serial.println(replybuffer);
#endif
  while(client->available()){
    client->read();
  }
  if(strstr(replybuffer,"NORMAL POWER DOWN")!=NULL){
    powerUpOrDown();
  }
  return replybuffer;
}