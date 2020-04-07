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
  _client_tcp = GPRS::getInstance() == NULL ? new GPRS(RX, TX, BAUDRATE) : GPRS::getInstance();
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

  if (!_preConnectionChecks()) {
    return false;
  }

  _client_tcp->send(payload);

  /* Parses the host answer, returns true if it is 'Ok' */
  char *response = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);

  float value = _parseTCPAnswer("POST", response);
  free(response);
  if (value != ERROR_VALUE) {
    _client_tcp->close();
    _client_tcp->disconnect();
    return true;
  }

  return false;
}

/**
 * @brief Get request over TCP to Ubidots server
 *
 * @param device_label device label of the device
 * @param variable_label variable label to be fetched
 * @return float Response value from the server
 */
float UbiTCP::get(const char *device_label, const char *variable_label) {

  if (!_preConnectionChecks()) {
    return ERROR_VALUE;
  }

  if (_debug) {
    Serial.println("waiting to fetch...");
  }

  uint16_t endpointLength = _endpointLength(device_label, variable_label);

  char *endpoint = (char *)malloc(sizeof(char) * endpointLength + 1);

  sprintf(endpoint, "%s|LV|%s|%s:%s|end HTTP/1.0\r\n\r\n", USER_AGENT, _token, device_label, variable_label);

  _client_tcp->send(endpoint, endpointLength);

  char *response = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);
  float value = _parseTCPAnswer("POST", response);

  _client_tcp->close();
  _client_tcp->disconnect();

  delay(200);

  free(endpoint);
  free(response);

  return value;
}

/**************************************************************************
 * Auxiliar
 ***************************************************************************/

/**
 * @brief Do the initialization of the device
 *
 * @return true The device is responding to the commands
 * @return false The device is not reponding, probably bad Serial communication
 */
bool UbiTCP::_initGPRS() {

  _guaranteePowerOn();

  uint8_t attempts = 0;
  bool flag = true;
  if (_debug) {
    Serial.print("Start TCP Connection...\r\n");
    Serial.print("Initializing module...\r\n");
  }

  // use DHCP
  while (!_client_tcp->init() && attempts < 5) {
    delay(1000);
    attempts += 1;
    if (attempts == 5) {
      flag = false;
      break;
    }

    if (_debug) {
      Serial.print("Checking Serial connection...\r\n");
    }
  }

  return flag;
}

/**
 * @brief Extablish the phisical connection with the GPRS network
 *
 * @return true The network is accepted by the device
 * @return false The device could connect to it, probably the frequency
 */
bool UbiTCP::_isNetworkRegistered() {

  uint8_t attempts = 0;
  bool flag = true;

  // use DHCP
  while (!_client_tcp->isNetworkRegistered() && attempts < 5) {
    delay(1000);
    attempts += 1;
    if (attempts == 5) {
      flag = false;
      break;
    }

    if (_debug) {
      Serial.println("Network has not been registered yet!");
    }
  }

  if (_debug) {
    Serial.println("Network has been registered into the module");
  }

  return flag;
}

/**
 * @brief The GPRS establish a new connection with the service
 * TODO cast the _apn, _apnUser, _apnPass to the join method.
 *
 */
bool UbiTCP::_isJoinedToNetwork() {

  uint8_t attempts = 0;
  bool flag = true;
  bool isJoined = false;

  // use DHCP
  while (!isJoined && attempts < 5) {
    isJoined = _client_tcp->join(_F(_apn), _F(_apnUser), _F(_apnPass));
    delay(2000);
    attempts += 1;
    if (attempts == 1) {
      if (_debug) {
        Serial.print("Network status: ");
        Serial.println(isJoined ? "Joined" : "Not Joined");
        Serial.println("Simcard Credentials");
        Serial.print("Apn: ");
        Serial.println(_apn);
        Serial.print("User: ");
        Serial.println(_apnUser);
        Serial.print("Pass: ");
        Serial.println(_apnPass);
        Serial.println();
      }
    }

    if (_debug) {
      Serial.print("Trying to join to the network [");
      Serial.print(attempts);
      Serial.println("]");
    }

    if (attempts == 5) {
      if (_debug) {
        Serial.println("[ERROR] Couldn't join the network\n\n");
      }
      flag = false;
      break;
    }
  }

  if (_debug && flag) {
    Serial.print("Network status: ");
    Serial.println(isJoined ? "Joined" : "Not Joined");
    // successful DHCP
    Serial.print("IP Address is ");
    Serial.println(_client_tcp->getIPAddress());

    if (!_checkIpAddress()) {
      Serial.println("IpAddres NOT VALID!");
      _isJoinedToNetwork();
    }
  }

  return flag;
}

/**
 * @brief Check the Octecs from the IP Address to make a valid request for the
 * IMEI, it splits the IP into tokes by the "." sparator, if there are 4 so it
 * is ok
 *
 * @return true There a valid connection to internet
 * @return false There is NOT a valid connection to internet
 */
bool UbiTCP::_checkIpAddress() {
  char *ipAddress = _client_tcp->getIPAddress();
  char *separator = ".";
  char *octet;

  uint8_t octecNumber = 0;
  octet = strtok(ipAddress, separator);

  while (octet != NULL) {
    octecNumber += 1;
    octet = strtok(NULL, separator);
  }

  if (octecNumber == 4) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Establish connection with Ubidots Server
 *
 * @return true Succesfull connection
 * @return false Failed connection
 */
bool UbiTCP::_connectToServer() {
  if (!_client_tcp->connect(TCP, UBI_INDUSTRIAL, UBIDOTS_TCP_PORT)) {
    if (_debug) {
      Serial.println("Error Connecting to Ubidots server");
      return false;
    }
  } else {
    _server_connected = true;
    if (_debug) {
      Serial.println("Connection to Ubidots server success!");
    }
    return true;
  }
}

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
    Serial.println("----------");
    Serial.println("Server's response:");
  }

  while (true) {
    int ret = _client_tcp->recv(response, MAX_BUFFER_SIZE);
    if (ret <= 0) {
      if (_debug) {
        Serial.println("fetch over...");
      }
      break;
    }
    response[ret] = '\0';
    if (_debug) {
      Serial.print("Recv: ");
      Serial.print(ret);
      Serial.print(" bytes: ");
      Serial.println(response);
    }
  }

  if (_debug) {
    Serial.println("\n----------");
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
 * @return false Server Obviusly not connected
 */
bool UbiTCP::serverConnected() { return _server_connected; }

/**
 * @brief Handle the proper behavior of the SIM900 before fetching or sending
 * data to Ubidots
 *
 * @return true Everything was fine
 * @return false something went wrong.
 */
bool UbiTCP::_preConnectionChecks() {

  if (!_initGPRS()) {
    return false;
  }

  if (!_isNetworkRegistered()) {
    return false;
  }

  if (!_isJoinedToNetwork()) {
    return false;
  }

  if (!_connectToServer()) {
    return false;
  }

  return true;
}

/**
 * @brief Check if the module is already powered on, if not then it tries to
 * power it forever.
 *
 */
void UbiTCP::_guaranteePowerOn() {

  if (_debug) {
    Serial.println("Cheking Power status SIM900 module");
  }

  if (!_client_tcp->checkPowerUp()) {
    pinMode(SIM900_POWER_UP_PIN, OUTPUT);
    if (_debug) {
      Serial.println("Turning on the SIM900 Module");
    }
    _client_tcp->powerUpDown();
    delay(2000);
    _guaranteePowerOn();
  } else {
    if (_debug) {
      Serial.println("SIM900 status: Powered On");
    }
  }
}
