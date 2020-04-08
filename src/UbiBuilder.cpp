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

#include "UbiBuilder.h"
#include "UbiTcp.h"

UbiBuilder::UbiBuilder(UbiToken token, UbiApn apn, UbiApn apnUser, UbiApn apnPass, UbiServer server,
                       IotProtocol iotProtocol) {
  _iotProtocol = iotProtocol;
  _server = server;
  _token = token;
  _apn = apn;
  _apnUser = apnUser;
  _apnPass = apnPass;
}

UbiProtocol *UbiBuilder::builder() {

  UbiProtocol *ubiBuilder = builderTcp();
  return ubiBuilder;
}

UbiProtocol *builderTcp() {
  Serial.println("Ubidots builderTcp");
  UbiProtocol *tcpInstance = new UbiTCP(_token, _server, UBIDOTS_TCP_PORT, USER_AGENT, _apn, _apnUser, _apnPass);
  return tcpInstance;
}
