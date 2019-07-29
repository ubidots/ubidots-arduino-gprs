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

#include "UbiTcp.h"
#include "UbiConstants.h"

/**************************************************************************
 * Overloaded constructors
 ***************************************************************************/

UbiTcp::UbiTcp(const char* token) {
  UbiTcp(UBI_INDUSTRIAL, UBIDOTS_TCP_PORT, UBIDOTS_USER_AGENT, token);
};
UbiTcp::UbiTcp(const int port, const char* user_agent, const char* token) {
  UbiTcp(UBI_INDUSTRIAL, port, user_agent, token);
};
UbiTcp::UbiTcp(const char* user_agent, const char* token) {
  UbiTcp(UBI_INDUSTRIAL, UBIDOTS_TCP_PORT, user_agent, token);
};
UbiTcp::UbiTcp(const char* host, const int port, const char* user_agent,
               const char* token) {
  _host = host;
  _port = port;
  _user_agent = user_agent;
  _token = token;
};

/**************************************************************************
 * Destructor
 ***************************************************************************/

UbiTcp::~UbiTcp() {
  delete[] _host;
  delete[] _user_agent;
  delete[] _token;
}

bool UbiTcp::sendData(const char* device_label, const char* device_name,
                      const char* payload) {}

float UbiTcp::get(const char* device_label, const char* variable_label) {}
