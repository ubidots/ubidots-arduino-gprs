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
                                Maria Carlina Hernandez
   https://github.com/mariacarlinahernandez
*/

#include "UbidotsArduinoGPRS.h"

/***************************************************************************
CONSTRUCTOR
***************************************************************************/

Ubidots::Ubidots(const char* token) {
  _token = token;
  _contextUbi = (ContextUbi*)malloc(MAX_VALUES * sizeof(ContextUbi));
  _dots = (Dot*)malloc(MAX_VALUES * sizeof(Dot));
}

/***************************************************************************
GPRS FUNCTIONS
***************************************************************************/

/***************************************************************************
FUNCTIONS TO RETRIEVE DATA
***************************************************************************/

/***************************************************************************
FUNCTIONS TO SEND DATA
***************************************************************************/

/***************************************************************************
AUXILIAR FUNCTIONS
***************************************************************************/

void Ubidots::setSerialParams(const uint8_t tx, const uint8_t rx,
                              uint32_t baudRate) {
  _tx = tx;
  _rx = rx;
  _baudRate = baudRate;
}

void Ubidots::setApnParams(const char* apn, const char* apnUsername,
                           const char* apnPassword) {
  _apn = apn;
  _apnUsername = apnUsername;
  _apnPassword = apnPassword;
}

void Ubidots::add(char* variableLabel, float dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}
void Ubidots::add(char* variableLabel, float dotValue, char* dotContext) {
  add(variableLabel, dotValue, dotContext, NULL, NULL);
}
void Ubidots::add(char* variableLabel, float dotValue, char* dotContext,
                  unsigned long dotTimestampSeconds) {
  add(variableLabel, dotValue, dotContext, dotTimestampSeconds, NULL);
}
void Ubidots::add(char* variableLabel, float dotValue, char* dotContext,
                  unsigned long dotTimestampSeconds,
                  unsigned int dotTimestampMillis) {
  (_dots + _currentDotValue)->variableLabel = variableLabel;
  (_dots + _currentDotValue)->dotValue = dotValue;
  (_dots + _currentDotValue)->dotContext = dotContext;
  (_dots + _currentDotValue)->dotTimestampSeconds = dotTimestampSeconds;
  (_dots + _currentDotValue)->dotTimestampMillis = dotTimestampMillis;
  _currentDotValue++;
  if (_currentDotValue > MAX_VALUES) {
    Serial.println(
        F("You are sending more than the maximum of consecutive variables"));
    _currentDotValue = MAX_VALUES;
  }
}
void Ubidots::addContext(char* key_label, char* key_value);
void Ubidots::getContext(char* context_result);
