/*
Copyright (c) 2013-2019 Ubidots.
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

#ifndef _UbiTcp_H_
#define _UbiTcp_H_

#include <Arduino.h>
#include <Stream.h>
#include <stdint.h>
#include <stdio.h>
#include "GPRS_Shield_Arduino.h"
#include "SoftwareSerial.h"
#include "Wire.h"

#include "UbiProtocol.h"
#include "UbiTypes.h"

class UbiTcp : public UbiProtocol {
 public:
  UbiTcp(const char* token, const uint8_t tx, const uint8_t rx,
         const uint32_t baudrate);
  void add(const char* variable_label, float value);
  void add(const char* variable_label, const char* value);
  void add(const char* variable_label, float value, const char* context);
  void add(const char* variable_label, float value, const char* context,
           unsigned long dot_timestamp_seconds);
  void add(const char* variable_label, float value, const char* context,
           unsigned long dot_timestamp_seconds,
           unsigned int dot_timestamp_millis);
  void add(const char* variable_label, const char* value, const char* context,
           unsigned long dot_timestamp_seconds,
           unsigned int dot_timestamp_millis);
  void addContext(const char* key_label, const char* key_value);
  void getContext(const char* context_result);
  /* Abstract methods */
  bool sendData(const char* device_label, const char* device_name);
  float get(const char* device_label, const char* variable_label);
  ~UbiTcp();

 private:
  const char* _token;
  Dot* _dots;
  uint8_t _decimalPrecision = 5;
  uint8_t _currentDotValue = 0;
  uint8_t _currentContextValue = 0;
  int _port;
  bool _moduleIsReady();
  void _buildTcpPayload(char* payload, const char* deviceLabel,
                        const char* deviceName);
  bool _connect();
  float _parseTCPAnswer(const char* request_type, char* response);
  GPRS* _gprs;
};

#endif
