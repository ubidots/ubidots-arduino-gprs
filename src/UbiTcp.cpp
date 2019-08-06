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

bool UbiTcp::sendData(const char* deviceLabel, const char* deviceName) {
  if (!_moduleIsReady()) return false;

  // Attempts to connect to Ubidots
  if (!_connect()) return false;

  // Sending data to Ubidots

  char* payload = (char*)malloc(sizeof(char) * MAX_BUFFER_SIZE);
  char buffer[512];
  _buildTcpPayload(payload, deviceLabel, deviceName);
  _gprs->send(payload, sizeof(payload) - 1);
  while (true) {
    int ret = _gprs->recv(buffer, sizeof(buffer) - 1);
    if (ret <= 0) {
      Serial.println("fetch over...");
      break;
    }
    buffer[ret] = '\0';
    Serial.print("Recv: ");
    Serial.print(ret);
    Serial.print(" bytes: ");
    Serial.println(buffer);
  }
  _gprs->close();
  _gprs->disconnect();

  free(payload);
}

float UbiTcp::get(const char* deviceLabel, const char* variableLabel) {
  if (!_moduleIsReady()) return ERROR_VALUE;
  /* Connecting the client */
  if (!_connect()) return false;

  /* Builds the request POST - Please reference this link to know all the
   * request's structures https://ubidots.com/docs/api/ */

  char payload[MAX_BUFFER_SIZE];
  sprintf(payload, "%s|LV|%s|%s:%s|end", UBIDOTS_USER_AGENT, _token,
          deviceLabel, variableLabel);

  Serial.println(payload);
  _gprs->send(payload, sizeof(payload) - 1);

  while (true) {
    int ret = _gprs->recv(payload, sizeof(payload) - 1);
    if (ret <= 0) {
      Serial.println("fetch over...");
      break;
    }
    payload[ret] = '\0';
    Serial.print("Recv: ");
    Serial.print(ret);
    Serial.print(" bytes: ");
    Serial.println(payload);
  }
  _gprs->close();
  _gprs->disconnect();

  /*_client_tcp_ubi.print(_user_agent);
  _client_tcp_ubi.print("|LV|");
  _client_tcp_ubi.print(_token);
  _client_tcp_ubi.print("|");
  _client_tcp_ubi.print(device_label);
  _client_tcp_ubi.print(":");
  _client_tcp_ubi.print(variable_label);
  _client_tcp_ubi.print("|end");

  if (_debug) {
    Serial.println("----");
    Serial.println("Payload for request:");
    Serial.print(_user_agent);
    Serial.print("|LV|");
    Serial.print(_token);
    Serial.print("|");
    Serial.print(device_label);
    Serial.print(":");
    Serial.print(variable_label);
    Serial.print("|end");
    Serial.println("\n----");
  }*/

  /* Waits for the host's answer */
  /*if (!waitServerAnswer()) {
    _client_tcp_ubi.stop();
    return ERROR_VALUE;
  }*/

  /* Reads the response from the server */
  /*char* response = (char*)malloc(sizeof(char) * MAX_BUFFER_SIZE);
  float value = parseTCPAnswer("LV", response);
  _client_tcp_ubi.stop();
  free(response);
  return value;*/
}

/**************************************************************************
 * Auxiliar Functions
 ***************************************************************************/

void UbiTcp::add(const char* variableLabel, float dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}

void UbiTcp::add(const char* variableLabel, const char* dotValue) {
  add(variableLabel, dotValue, NULL, NULL, NULL);
}

void UbiTcp::add(const char* variableLabel, float dotValue,
                 const char* dotContext) {
  add(variableLabel, dotValue, dotContext, NULL, NULL);
}

void UbiTcp::add(const char* variableLabel, float dotValue,
                 const char* dotContext, unsigned long dotTimestampSeconds) {
  add(variableLabel, dotValue, dotContext, dotTimestampSeconds, NULL);
}

void UbiTcp::add(const char* variableLabel, float dotValue,
                 const char* dotContext, unsigned long dotTimestampSeconds,
                 unsigned int dotTimestampMillis) {
  char buf[33];
  dtostrf(dotValue, (_decimalPrecision + 2), _decimalPrecision, buf);
  const char* ptr = buf;
  add(variableLabel, ptr, dotContext, dotTimestampSeconds, dotTimestampMillis);
}

void UbiTcp::add(const char* variableLabel, const char* dotValue,
                 const char* dotContext, unsigned long dotTimestampSeconds,
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

bool UbiTcp::_moduleIsReady() {
  _gprs->checkPowerUp();

  // Checks if module is turned on
  uint8_t attempts = 0;
  Serial.println("Checking if GPRS is on");
  while (!_gprs->init() && attempts < 5) {
    Serial.print(".");
    attempts += 1;
    delay(1000);
  }
  if (!_gprs->init()) {
    Serial.println("Could not initialize the GPRS module");
    return false;
  }
  Serial.println("finished");

  // Checks if there is sim card inside the shield
  attempts = 0;
  Serial.println("Checking if there is a connected SIM");
  while (!_gprs->isNetworkRegistered() && attempts < 5) {
    Serial.print(".");
    attempts += 1;
    delay(1000);
  }
  if (!_gprs->isNetworkRegistered()) {
    Serial.println("Could not detect a connected SIM");
    return false;
  }
  Serial.println("finished");

  // Attempts to Join to GPRS network
  attempts = 0;
  Serial.println("Attempting to open socket with GPRS service provider");
  while (!_gprs->join(F("aa")) && attempts < 5) {
    Serial.print(".");
    attempts += 1;
    delay(1000);
  }
  if (!_gprs->join(F("aa"))) {
    Serial.println(
        "Could not connect, please check your APN and username settings");
    return false;
  }

  return true;
}

bool UbiTcp::_connect() {
  uint8_t attempts = 0;
  Serial.println("Attempting to connect to Ubidots");
  while (!_gprs->connect(TCP, UBI_INDUSTRIAL, UBIDOTS_TCP_PORT) &&
         attempts < 5) {
    Serial.print(".");
    attempts += 1;
    delay(1000);
  }
  if (!_gprs->connect(TCP, UBI_INDUSTRIAL, UBIDOTS_TCP_PORT)) {
    Serial.println(
        "Could not connect to Ubidots, please check your device cloud "
        "connection");
    return false;
  }
  Serial.println("finished");
  return true;
}

/**
 * Parse the TCP host answer and saves it to the input char pointer.
 * @payload [Mandatory] char payload pointer to store the host answer.
 * @return true if there is an 'Ok' in the answer, false if not.
 */

float UbiTcp::_parseTCPAnswer(const char* request_type, char* response) {
  int j = 0;
  Serial.println("----------");
  Serial.println("Server's response:");

  float result = ERROR_VALUE;

  // POST
  if (request_type == "POST") {
    char* pch = strstr(response, "OK");
    if (pch != NULL) {
      result = 1;
    }
    return result;
  }

  // LV
  char* pch = strchr(response, '|');
  if (pch != NULL) {
    result = atof(pch + 1);
  }

  return result;
}
