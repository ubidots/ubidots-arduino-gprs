/*
Copyright (c) 2013-2019 UbiTcp.
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

Developed and maintained by Jose Garcia for IoT Services Inc
@jotathebest at github: https://github.com/jotathebest
*/

#include "UbiTcp.h"
#include "UbiConstants.h"

/**************************************************************************
 * Overloaded constructors
 ***************************************************************************/

UbiTcp::UbiTcp(const char* token, const uint8_t tx, const uint8_t rx,
               const uint32_t baudrate) {
  _token = token;
  _dots = (Dot*)malloc(MAX_VALUES * sizeof(Dot));
  _gprs = new GPRS(tx, rx, baudrate);
};

/**************************************************************************
 * Destructor
 ***************************************************************************/

UbiTcp::~UbiTcp() {
  delete[] _token;
  delete _gprs;
  free(_dots);
}

bool UbiTcp::sendData(const char* deviceLabel, const char* deviceName) {}

float UbiTcp::get(const char* deviceLabel, const char* variable_label) {}

/**************************************************************************
 * Auxiliar Functions
 ***************************************************************************/

void UbiTcp::add(char* variableLabel, float dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}
void UbiTcp::add(char* variableLabel, float dotValue, char* dotContext) {
  add(variableLabel, dotValue, dotContext, NULL, NULL);
}
void UbiTcp::add(char* variableLabel, float dotValue, char* dotContext,
                 unsigned long dotTimestampSeconds) {
  add(variableLabel, dotValue, dotContext, dotTimestampSeconds, NULL);
}

void UbiTcp::add(char* variableLabel, float dotValue, char* dotContext,
                 unsigned long dotTimestampSeconds,
                 unsigned int dotTimestampMillis) {
  char buf[33];
  dtostrf(dotValue, (_decimalPrecision + 2), _decimalPrecision, buf);

  add(variableLabel, buf, dotContext, dotTimestampSeconds, dotTimestampMillis);
}

void UbiTcp::add(char* variableLabel, const char* dotValue, char* dotContext,
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

void UbiTcp::_buildTcpPayload(char* payload, const char* deviceLabel,
                              const char* deviceName) {
  sprintf(payload, "");
  sprintf(payload, "%s|POST|%s|", UBIDOTS_USER_AGENT, _token);
  sprintf(payload, "%s%s:%s", payload, deviceLabel, deviceName);

  sprintf(payload, "%s=>", payload);
  for (uint8_t i = 0; i < _currentDotValue;) {
    sprintf(payload, "%s%s:%s", payload, (_dots + i)->variableLabel,
            (_dots + i)->dotValue);

    // Adds dot context
    if ((_dots + i)->dotContext != NULL) {
      sprintf(payload, "%s$%s", payload, (_dots + i)->dotContext);
    }

    // Adds timestamp seconds
    if ((_dots + i)->dotTimestampSeconds != NULL) {
      sprintf(payload, "%s@%lu", payload, (_dots + i)->dotTimestampSeconds);
      // Adds timestamp milliseconds
      if ((_dots + i)->dotTimestampMillis != NULL) {
        char milliseconds[3];
        int timestamp_millis = (_dots + i)->dotTimestampMillis;
        uint8_t units = timestamp_millis % 10;
        uint8_t dec = (timestamp_millis / 10) % 10;
        uint8_t hund = (timestamp_millis / 100) % 10;
        sprintf(milliseconds, "%d%d%d", hund, dec, units);
        sprintf(payload, "%s%s", payload, milliseconds);
      } else {
        sprintf(payload, "%s000", payload);
      }
    }

    i++;

    if (i < _currentDotValue) {
      sprintf(payload, "%s,", payload);
    } else {
      sprintf(payload, "%s|end", payload);
      _currentDotValue = 0;
    }
  }
}
