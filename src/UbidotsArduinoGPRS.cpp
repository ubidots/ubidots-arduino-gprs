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

    Created and maintained by:  Jose Garcia https://github.com/jotathebest
*/

#include "UbidotsArduinoGPRS.h"

/***************************************************************************
CONSTRUCTOR
***************************************************************************/

Ubidots::Ubidots(const char* token, const uint8_t tx, const uint8_t rx,
                 const uint32_t baudRate) {
  _token = token;
  _contextUbi = (ContextUbi*)malloc(MAX_VALUES * sizeof(ContextUbi));
  _ubiTcpClient = new UbiTcp(_token, tx, rx, baudRate);
}

/**************************************************************************
 * Destructor
 ***************************************************************************/

Ubidots::~Ubidots() {
  delete[] _token;
  free(_contextUbi);
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

bool Ubidots::send() { send(_defaultDeviceLabel, _defaultDeviceLabel); }

bool Ubidots::send(const char* deviceLabel) { send(deviceLabel, deviceLabel); }

bool Ubidots::send(const char* deviceLabel, const char* deviceName) {
  _ubiTcpClient->sendData(deviceLabel, deviceName);
}

/***************************************************************************
AUXILIAR FUNCTIONS
***************************************************************************/

void Ubidots::add(const char* variableLabel, float dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}

void Ubidots::add(const char* variableLabel, const char* dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}

void Ubidots::add(const char* variableLabel, float dotValue,
                  const char* dotContext) {
  add(variableLabel, dotValue, dotContext, NULL, NULL);
}
void Ubidots::add(const char* variableLabel, float dotValue,
                  const char* dotContext, unsigned long dotTimestampSeconds) {
  add(variableLabel, dotValue, dotContext, dotTimestampSeconds, NULL);
}

void Ubidots::add(const char* variableLabel, float dotValue,
                  const char* dotContext, unsigned long dotTimestampSeconds,
                  unsigned int dotTimestampMillis) {
  _ubiTcpClient->add(variableLabel, dotValue, dotContext, dotTimestampSeconds,
                     dotTimestampMillis);
}

void Ubidots::add(const char* variableLabel, const char* dotValue,
                  const char* dotContext, unsigned long dotTimestampSeconds,
                  unsigned int dotTimestampMillis) {
  _ubiTcpClient->add(variableLabel, dotValue, dotContext, dotTimestampSeconds,
                     dotTimestampMillis);
}

void Ubidots::addContext(const char* key_label, const char* key_value) {}
void Ubidots::getContext(const char* context_result) {}
