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

#ifndef __UbidotsArduinoGPRS_H_
#define __UbidotsArduinoGPRS_H_

#include <Arduino.h>
#include <Stream.h>
#include <stdint.h>
#include <stdio.h>
#include "GPRS_Shield_Arduino.h"
#include "SoftwareSerial.h"
#include "Wire.h"

#include "UbiConstants.h"
#include "UbiProtocol.h"
#include "UbiTcp.h"
#include "UbiTypes.h"
#include "UbidotsArduinoGPRS.h"

class Ubidots {
 public:
  Ubidots(const char* token, const uint8_t tx = 7, const uint8_t rx = 8,
          const uint32_t _baudRate = 9600);
  void add(char* variable_label, float dotValue);
  void add(char* variable_label, float dotValuevalue, char* context);
  void add(char* variable_label, float dotValue, char* context,
           unsigned long dot_timestamp_seconds);
  void add(char* variable_label, float dotValue, char* context,
           unsigned long dot_timestamp_seconds,
           unsigned int dot_timestamp_millis);
  void add(char* variable_label, const char* dotValue, char* context,
           unsigned long dot_timestamp_seconds,
           unsigned int dot_timestamp_millis);
  void addContext(char* key_label, char* key_value);
  void getContext(char* context_result);
  bool send();
  bool send(const char* device_label);
  bool send(const char* device_label, const char* device_name);
  ~Ubidots();

 private:
  uint8_t _decimalPrecision = 5;
  char* _token;
  char* _defaultDeviceLabel = "gprs";
  ContextUbi* _contextUbi;
  void _buildTcpPayload(char* payload, const char* device_label,
                        const char* device_name);
  UbiTcp* _ubiTcpClient;
};

#endif
