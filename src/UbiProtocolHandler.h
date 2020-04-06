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

Developed and maintained by Jose Garcia and Cristian Arrieta for IoT Services
Inc
@jotathebest at github: https://github.com/jotathebest
@crisap94 at github: https://github.com/crisap94
*/

#ifndef _UbiProtocolHandler_H_
#define _UbiProtocolHandler_H_

#include "UbiBuilder.h"

class UbiProtocolHandler {

private:
  int _connectionTimeout = 5000;
  uint8_t _maxConnectionAttempts = 20;
  int8_t _current_value = 0;

  UbiToken _token;
  char _defaultDeviceLabel[18];
  bool _debug;

  UbiProtocol *_ubiProtocol;
  IotProtocol _iotProtocol;
  Value *_dots;

  void buildHttpPayload(char *payload);
  void buildTcpPayload(char *payload, const char *device_label, const char *device_name);
  void _builder(UbiToken token, UbiApn apn, UbiApn apnUser, UbiApn apnPass, UbiServer server = UBI_INDUSTRIAL,
                IotProtocol iotProtocol = UBI_TCP);

  void _floatToChar(char *value_str, float value);

public:
  explicit UbiProtocolHandler(UbiToken token, UbiApn apn, UbiApn apnUser, UbiApn apnPass,
                              UbiServer server = UBI_INDUSTRIAL, IotProtocol iotProtocol = UBI_TCP);

  void add(const char *variable_label, float value, char *context, unsigned long dot_timestamp_seconds,
           unsigned int dot_timestamp_millis);

  bool send(const char *device_label, const char *device_name);

  float get(const char *device_label, const char *variable_label);

  void setDebug(bool debug);

  bool serverConnected();
  ~UbiProtocolHandler();
};

#endif
