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

#include "UbiTcp.h"

/**************************************************************************
 * Overloaded constructors
 ***************************************************************************/

/**
 * @brief Construct a new UbiTCP object and power up the module
 *
 * @param token Users token
 * @param server Server addres
 * @param port Server port
 * @param user_agent Reference from the device being connected
 * @param apn GPRS network name
 * @param apnUser GPRS network username
 * @param apnPass GPRS network passwrod
 */
UbiTCP::UbiTCP(UbiToken token, UbiServer server, const int port, const char *user_agent, UbiApn apn, UbiApn apnUser,
               UbiApn apnPass) {

  _server = server;
  _user_agent = user_agent;
  _token = token;
  _port = port;
  _apn = apn;
  _apnUser = apnUser;
  _apnPass = apnPass;
  _client_tcp = new GPRS(RX, TX, BAUDRATE);
}

/**************************************************************************
 * Destructor
 ***************************************************************************/

UbiTCP::~UbiTCP() {
  delete[] _server;
  delete[] _user_agent;
  delete[] _token;
  delete[] _apn;
  delete[] _apnUser;
  delete[] _apnPass;

  free(_client_tcp);
}

/**************************************************************************
 * Cloud Functions
 ***************************************************************************/

/**
 * @brief Post new data to ubidots server
 *
 * @param device_label Label of the device to be set or update in the ubidots
 * platform
 * @param device_name Device name to be published
 * @param payload Body to be processed by the Ubidot server
 * @return true The data was succesfully published
 * @return false Something went wrong with the sending.
 */
bool UbiTCP::sendData(const char *device_label, const char *device_name, char *payload) {
  _client_tcp->wake();

  if (!_preConnectionChecks()) {
    return false;
  }

  _client_tcp->send(payload);

  /* Parses the host answer, returns true if it is 'Ok' */
  char *response = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);

  float value = _parseTCPAnswer("POST", response);
  free(response);

  _client_tcp->sleep();

  return value != ERROR_VALUE;
}

/**
 * @brief Get request over TCP to Ubidots server
 *
 * @param device_label device label of the device
 * @param variable_label variable label to be fetched
 * @return float Response value from the server
 */
float UbiTCP::get(const char *device_label, const char *variable_label) {
  _client_tcp->wake();

  if (!_preConnectionChecks()) {
    return ERROR_VALUE;
  }

  if (_debug) {
    Serial.println(F("waiting to fetch..."));
  }

  uint16_t endpointLength = _endpointLength(device_label, variable_label);

  char *endpoint = (char *)malloc(sizeof(char) * endpointLength + 1);

  sprintf(endpoint, "%s|LV|%s|%s:%s|end HTTP/1.0\r\n\r\n", USER_AGENT, _token, device_label, variable_label);

  if (_debug) {
    Serial.print(F("Request: "));
    Serial.println(endpoint);
  }

  _client_tcp->send(endpoint, endpointLength);
  free(endpoint);

  char *response = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);
  float value = _parseTCPAnswer("POST", response);

  free(response);

  _client_tcp->sleep();

  return value;
}
/**************************************************************************
 * Auxiliar Cloud Functions
 ***************************************************************************/

/**
 * @brief Calculate the lenght of the enpoint to be send over TCP to the server
 *
 * @param device_label device label of the device
 * @param variable_label variable label to be updated or fetched
 * @return uint16_t  Lenght of the enpoint
 */
uint16_t UbiTCP::_endpointLength(const char *device_label, const char *variable_label) {
  uint16_t endpointLength = strlen("|LV||:|end HTTP/1.0\r\n\r\n") + strlen(USER_AGENT) + strlen(_token) +
                            strlen(device_label) + strlen(variable_label);
  return endpointLength;
}

/**
 * @brief Parse the TCP host answer and saves it to the input char pointer.

 * @param request_type Type of request GET or POST
 * @param response [Mandatory] Buffer to contain the payload from the server
 * @return float Value from the response
 */
float UbiTCP::_parseTCPAnswer(const char *request_type, char *response) {
  int j = 0;

  if (_debug) {
    Serial.println(F("----------"));
    Serial.println(F("Server's response:"));
  }

  while (true) {
    int ret = _client_tcp->recv(response, MAX_BUFFER_SIZE);
    if (ret <= 0) {
      if (_debug) {
        Serial.println(F("fetch over..."));
      }
      break;
    }
    response[ret] = '\0';
    if (_debug) {
      Serial.print(F("Recv: "));
      Serial.print(ret);
      Serial.print(F(" bytes: "));
      Serial.println(response);
    }
  }

  if (_debug) {
    Serial.println(F("\n----------"));
  }

  response[j] = '\0';
  float result = ERROR_VALUE;

  // POST
  if (strcmp(request_type, "POST") == 0) {
    char *pch = strstr(response, "OK");
    if (pch != NULL) {
      result = 1;
    }
    return result;
  }

  // LV
  char *pch = strchr(response, '|');
  if (pch != NULL) {
    result = atof(pch + 1);
  }

  return result;
}

/**************************************************************************
 * Auxiliar GPRS functions
 ***************************************************************************/
/**
 * @brief Check if the module is already powered on, if ! then it tries to
 * power it forever.
 *
 */
bool UbiTCP::_isPoweredOn() {
  if (!isPoweredOn) {
    if (_debug) {
      Serial.println(F("Checking Power status SIM900 module"));
      Serial.println(F("Sim900 is Powered Off"));
    }
    while (!isPoweredOn) {
      isPoweredOn = _client_tcp->checkPowerUp();
      pinMode(SIM900_POWER_UP_PIN, OUTPUT);
      if (_debug) {
        Serial.println(F("Turning on SIM900"));
      }
      if (!isPoweredOn) {
        _client_tcp->powerUpDown();
      }
    }
  }

  if (_debug) {
    Serial.println(F("Module Powered ON"));
  }

  return isPoweredOn;
}

/**
 * @brief Verify the status from the Simcard
 *
 * @return true The Simcard is correctle placed
 * @return false The Sim Card is not inserted correctly
 */
bool UbiTCP::_isSimCardInserted() {
  if (!isSimInserted) {
    isSimInserted = _client_tcp->checkSIMStatus();

    if (_debug) {
      Serial.println(isSimInserted ? F("Sim Card Correctly installed") : F("[ERROR] Sim Card ! FOUND"));
    }
  }

  return isSimInserted;
}

/**
 * @brief Do the initialization of the device
 *
 * @return true The device is responding to the commands
 * @return false The device is ! reponding, probably bad Serial communication
 */
bool UbiTCP::_initGPRS() {

  if (!isInitiatedModule) {
    const uint8_t MAX_INIT_ATTEMPS = 5;
    uint8_t attempts = 0;
    isInitiatedModule = true;
    if (_debug) {
      Serial.print(F("Initializing module...\r\n"));
    }

    // use DHCP
    while (!_client_tcp->init() && attempts < MAX_INIT_ATTEMPS) {

      attempts += 1;
      if (attempts == MAX_INIT_ATTEMPS) {
        isInitiatedModule = false;
      }
      if (_debug) {
        Serial.print(F("Checking Serial connection ["));
        Serial.print(attempts);
        Serial.println(F("]"));
      }
    }
  } else {
    if (_debug) {
      Serial.println(F("Module already initialized"));
    }
  }

  return isInitiatedModule;
}

/**
 * @brief Extablish the phisical connection with the GPRS network
 *
 * @return true The network is accepted by the device
 * @return false The device could connect to it, probably the frequency
 */
bool UbiTCP::_isNetworkRegistered() {
  if (!isNetworkRegistered) {
    const uint8_t MAX_REGISTER_ATTEMPS = 5;
    if (_debug) {
      Serial.println(F("Registering Network into the module"));
    }
    uint8_t attempts = 0;
    isNetworkRegistered = true;

    // use DHCP
    while (!_client_tcp->isNetworkRegistered() && attempts < MAX_REGISTER_ATTEMPS) {

      yield();
      attempts += 1;
      if (attempts == MAX_REGISTER_ATTEMPS) {
        isNetworkRegistered = false;
      }

      if (_debug) {
        Serial.print(F("Trying to register the network ["));
        Serial.print(attempts);
        Serial.println(F("]"));
      }
    }

    if (_debug) {
      Serial.println(isNetworkRegistered ? F("Network has been registered into the module")
                                         : F("[ERROR] Couldn't register into the network\n\n"));
    }

  } else {

    if (_debug) {
      Serial.println(F("Network already registered into the device"));
    }
  }

  return isNetworkRegistered;
}

/**
 * @brief The GPRS establish a new connection with the service
 * TODO cast the _apn, _apnUser, _apnPass to the join method.
 *
 */
bool UbiTCP::_isJoinedToNetwork() {
  if (!isJoinedToNetwork) {
    const uint8_t MAX_JOIN_ATTEMPS = 5;
    if (_debug) {
      Serial.println(F("Joining module to the network"));
    }
    uint8_t attempts = 0;
    isJoinedToNetwork = true;

    if (_debug) {
      Serial.println(F("Simcard Credentials"));
      Serial.print(F("Apn: "));
      Serial.println(_apn);
      Serial.print(F("User: "));
      Serial.println(_apnUser);
      Serial.print(F("Pass: "));
      Serial.println(_apnPass);
      Serial.println();
    }
    // use DHCP
    while (!_client_tcp->join(_F(_apn), _F(_apnUser), _F(_apnPass)) && attempts < MAX_JOIN_ATTEMPS) {

      yield();
      attempts += 1;
      if (attempts == MAX_JOIN_ATTEMPS) {
        isJoinedToNetwork = false;
      }

      if (_debug) {
        int signal = 0;
        _client_tcp->getSignalStrength(&signal);
        Serial.print(F("Trying to join to the network ["));
        Serial.print(attempts);
        Serial.print(F("] "));
        Serial.print(F("Signal: "));
        Serial.print(F("-"));
        Serial.print(signal);
        Serial.println(signal < 75 ? F(" [EXCELLENT]")
                                   : signal < 85 ? F(" [GOOD]") : signal < 95 ? F(" [OK]") : F(" [WEAK]"));
      }
    }

    if (_debug) {
      Serial.print(F("Network status: "));
      Serial.println(isJoinedToNetwork ? F("Joined") : F("[ERROR] Couldn't join the network\n\n"));
    }

  } else {
    if (_debug) {
      Serial.println(F("Module already joined to the network"));
    }
  }

  return isJoinedToNetwork;
}

/**
 * @brief Establish connection with Ubidots Server
 *
 * @return true Succesfull connection
 * @return false Failed connection
 */
bool UbiTCP::_isConnectedToServer() {
  if (_debug) {
    Serial.print("Start TCP Connection...\r\n");
  }

  if (!_client_tcp->is_connected()) {

    isServerConected = _client_tcp->connect(TCP, UBI_INDUSTRIAL, UBIDOTS_TCP_PORT);

    if (_debug) {
      Serial.println(isServerConected ? F("Connection to Ubidots server success!")
                                      : F("Error Connecting to Ubidots server"));
    }

  } else {
    if (_debug) {
      Serial.println(F("Connection to Ubidots server already established!"));
    }
  }
  return isServerConected;
}

/**************************************************************************
 * Auxiliar Ubidots function
 ***************************************************************************/

/**
 * @brief Enable the debug
 *
 * @param debug Makes available debug traces
 */
void UbiTCP::setDebug(bool debug) { _debug = debug; }

/**
 * @brief Checks if the socket is still opened with the Ubidots Server through
 * the attribute
 *
 * @return true Server Connected
 * @return false Server Obviusly ! connected
 */
bool UbiTCP::serverConnected() { return isServerConected; }

/**
 * @brief Handle the proper behavior of the SIM900 before fetching or sending
 * data to Ubidots
 *
 * @return true Everything was fine
 * @return false something went wrong.
 */
bool UbiTCP::_preConnectionChecks() {

  if (!_isPoweredOn()) {
    return false;
  }

  if (!_isSimCardInserted()) {
    return false;
  }

  if (!_initGPRS()) {
    return false;
  }

  if (!_isNetworkRegistered()) {
    return false;
  }

  if (!_isJoinedToNetwork()) {
    return false;
  }

  if (!_isConnectedToServer()) {
    return false;
  }

  return true;
}
