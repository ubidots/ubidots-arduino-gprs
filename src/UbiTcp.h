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

#ifndef _UbiTcp_H_
#define _UbiTcp_H_

#include <GPRS_Shield_Arduino.h>

#include "UbiProtocol.h"

#define _F(string) reinterpret_cast<__FlashStringHelper *>(const_cast<char *>(string))

#define RX 7
#define TX 8
#define BAUDRATE 19200
#define SIM900_POWER_UP_PIN 9
#define APN "web.colombiamovil.com.co"

class UbiTCP : public UbiProtocol {

private:
  bool _debug = false;
  int _timeout = 5000;
  unsigned long _timerToSync = millis();
  bool _server_connected = false;

  GPRS *_client_tcp;

  UbiServer _server;
  const char *_user_agent;
  UbiToken _token;
  UbiApn _apn;
  UbiApn _apnUser;
  UbiApn _apnPass;

  int _port;

  float _parseTCPAnswer(const char *request_type, char *response);
  bool _initGPRS();
  bool _isNetworkRegistered();
  bool _isJoinedToNetwork();
  bool _connectToServer();
  bool _checkIpAddress();
  void _guaranteePowerOn();

  uint16_t _endpointLength(const char *device_label,
                           const char *variable_label);

  bool _preConnectionChecks();

public:
  UbiTCP(UbiToken token, UbiServer server, const int port,
         const char *user_agent, UbiApn apn, UbiApn apnUser, UbiApn apnPass);

  bool sendData(const char *device_label, const char *device_name,
                char *payload);

  float get(const char *device_label, const char *variable_label);

  void setDebug(bool debug);

  bool serverConnected();

  ~UbiTCP();
};

#endif
