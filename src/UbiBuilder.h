/*
Copyright (c) 2013-2018 Ubidots.
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

#ifndef _UbiBuilder_H_
#define _UbiBuilder_H_

#include "UbiProtocol.h"

#include <stdint.h>
#include <stdio.h>

UbiProtocol *builderTcp();

namespace {
UbiServer _server;
UbiToken _token;
UbiApn _apn;
UbiApn _apnUser;
UbiApn _apnPass;
} // namespace

class UbiBuilder {

private:
  IotProtocol _iotProtocol;

public:

  explicit UbiBuilder(UbiToken token, UbiApn apn, UbiApn apnUser,
                      UbiApn apnPass, UbiServer server,
                      IotProtocol iotProtocol);

  UbiProtocol *builder();

};

#endif
