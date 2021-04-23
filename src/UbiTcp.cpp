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

  Sim900 = new SoftwareSerial(RX, TX);  // RX, TX

  Sim900->begin(BAUDRATE);
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

  free(Sim900);
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

  if (!sendCommandToServer(payload)) return false;

  float value = _parseTCPAnswer("POST");

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
  if (!_preConnectionChecks()) {
    return ERROR_VALUE;
  }

  uint16_t endpointLength = _endpointLength(device_label, variable_label);

  char *requestLine = (char *)malloc(sizeof(char) * endpointLength + 1);

  sprintf(requestLine, "%s|LV|%s|%s:%s|end", USER_AGENT, _token, device_label, variable_label);

  if (_debug) {
    Serial.print(F("Request: "));
    Serial.println(requestLine);
  }

  if (!sendCommandToServer(requestLine)) return ERROR_VALUE;

  free(requestLine);
  float value = _parseTCPAnswer("LV");

  // Flush Connection

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
  uint16_t endpointLength =
      strlen("|LV||:|end") + strlen(USER_AGENT) + strlen(_token) + strlen(device_label) + strlen(variable_label);
  return endpointLength;
}

/**
 * @brief Parse the TCP host answer and saves it to the input char pointer.

 * @param request_type Type of request GET or POST
 * @param response [Mandatory] Buffer to contain the payload from the server
 * @return float Value from the response
 */
float UbiTCP::_parseTCPAnswer(const char *request_type) {
  char *ptr = strstr(replybuffer, "OK");

  if (ptr == NULL) return ERROR_VALUE;

  char *serverResponse = ptr + 4;

  if (_debug) {
    Serial.println(F("----------"));
    Serial.println(F("Server's response:"));
  }

  if (_debug) {
    Serial.println(serverResponse);
    Serial.println(F("----------"));
  }

  float result = ERROR_VALUE;

  // POST
  if (strcmp(request_type, "POST") == 0) {
    char *pch = strstr(serverResponse, "OK");
    if (pch != NULL) {
      result = 1;
    }
    return result;
  }

  // LV
  char *pch = strchr(serverResponse, '|');
  if (pch != NULL) {
    result = atof(pch + 1);
  }

  return result;
}

/**************************************************************************
 * Auxiliar GPRS functions
 ***************************************************************************/

void UbiTCP::_powerUpDown() {
  // power on pulse for SIM900 Shield
  digitalWrite(SIM900_POWER_UP_PIN, LOW);
  delay(1000);
  digitalWrite(SIM900_POWER_UP_PIN, HIGH);
  delay(2000);
  digitalWrite(SIM900_POWER_UP_PIN, LOW);
  delay(3000);
}

/**
 * @brief Check if the module is already powered on, if not then it tries to
 * power it forever.
 *
 */
bool UbiTCP::_isPoweredOn() {
  if (!isPoweredOn) {
    if (_debug) {
      Serial.println(F("Checking Power status SIM900 module"));
      Serial.println(F("Sim900 is Powered Off"));
      Serial.println(F("Powering on SIM900"));
    }
    pinMode(SIM900_POWER_UP_PIN, OUTPUT);

    isPoweredOn = sendCommand("AT", "OK");

    while (!isPoweredOn) {
      _powerUpDown();
      isPoweredOn = sendCommand("AT", "OK");
    }
  }

  if (_debug && isPoweredOn) {
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
    isSimInserted = sendCommand("AT+CPIN?", "READY");

    if (_debug) {
      Serial.println(isSimInserted ? F("Sim Card Correctly installed") : F("[ERROR] Sim Card NOT FOUND"));
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
    if (_debug) {
      Serial.print(F("Initializing module...\r\n"));
    }

    if (sendCommand("AT+CIPSTATUS", "CONNECT OK", 2000) || sendCommand("AT+CIPSTATUS", "TCP CLOSED", 2000)) {
      isInitiatedModule = true;
      isNetworkRegistered = true;
      isJoinedToNetwork = true;
      return true;
    }

    bool CIPMODE = sendCommand("AT+CIPMODE=0", "OK");      // Set Client to non-transparent mode
    bool CIPMUX = sendCommand("AT+CIPMUX=0", "OK", 1000);  // Config device to Single Client

    isInitiatedModule = CIPMODE && CIPMUX;

    if (_debug) {
      Serial.println(isInitiatedModule ? F("Initialization Completed") : F("Initialization Failed"));
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
    uint8_t MAX_ATTEMPS = 3;
    uint8_t attemps = 0;

    if (_debug) {
      Serial.println(F("Registering module to the mobile Network"));
    }

    while (!isNetworkRegistered && attemps < MAX_ATTEMPS) {
      /* code */

      bool CGATT = sendCommand("AT+CGATT=1", "OK", 2000);
      bool IP_INITIAL = sendCommand("AT+CIPSTATUS", "IP INITIAL", 2000);

      sendCommand("AT+CREG?");

      //  Check wether the device is attach to GPRS network
      //  REG_OK_HOME = 1
      //  REG_OK_ROAMING = 5
      isNetworkRegistered = (
          strstr(replybuffer, "0,1") != NULL) 
          || strstr(replybuffer, "1,1") != NULL) 
          || (strstr(replybuffer, "0,5") != NULL
        );
      isNetworkRegistered = isNetworkRegistered && CGATT && IP_INITIAL;
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
 * @brief Check for a proper connection to the network (IP correct)
 *
 * @return true There is ping
 * @return false There is no Ping
 */
bool UbiTCP::_hasConnectivity() {
  if (_debug) Serial.println(F("Verifying network connectivity on SIM900"));

  // Checks for the  IP Address
  // Madatory to enable the IP STACK
  sendCommand("AT+CIFSR");

  if (_debug) {
    Serial.println(F("IP Address: "));
    Serial.print(replybuffer);  // Print the IP Address
  }

  return sendCommand("AT+CIPSTATUS", "IP STATUS");  // Check if it was succesfull the IP STACK initialization
}

/**
 * @brief The GPRS establish a new connection with the service
 *
 */
bool UbiTCP::_isJoinedToNetwork() {
  if (!isJoinedToNetwork) {
    if (_debug) {
      Serial.println(F("Authenticating module to network"));
    }

    /**
     * Set the ip stack available to proceed to the configuration.
     * Not needed to check the status
     */
    if (!sendCommand("AT+CGATT?", "+CGATT: 1")) return false;

    sendCommand("AT+CIPSHUT", "OK");  // Ready to configure

    const char *AT = "AT+CSTT";

    char *apnCommand =
        (char *)malloc(sizeof(char) * (strlen(AT) + strlen(_apn) + strlen(_apnUser) + strlen(_apnPass)) + 13);

    sprintf(apnCommand, "%s=\"%s\",\"%s\",\"%s\"", AT, _apn, _apnUser, _apnPass);

    bool apnHandshake = sendCommand(apnCommand, "OK");

    if (_debug) {
      Serial.println(apnHandshake ? F("APN set Correctly") : F("APN set Failed"));
    }

    isJoinedToNetwork = sendCommand("AT+CIICR", "OK", 2000) && apnHandshake;

    if (!_hasConnectivity()) return false;

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
    Serial.print(F("Start TCP Connection...\r\n"));
  }

  if (sendCommand("AT+CIPSTATUS", "CONNECT OK")) {
    if (_debug) Serial.println("Already Connected to Server");
    return true;
  }

  if (sendCommand("AT+CIPSTATUS", "TCP CLOSED") || sendCommand("AT+CIPSTATUS", "IP STATUS")) {
    const char *AT = "AT+CIPSTART=\"TCP\"";
    char *serverCommand = (char *)malloc(sizeof(char) * (strlen(AT) + strlen(UBI_INDUSTRIAL) + 4 + 10));

    sprintf(serverCommand, "%s,\"%s\",\"%i\"", AT, UBI_INDUSTRIAL, UBIDOTS_TCP_PORT);

    isConnectedToServer = sendCommand(serverCommand, "CONNECT OK", 4000);

    if (_debug) {
      Serial.println(isConnectedToServer ? F("Connection to Ubidots server success!")
                                         : F("Error Connecting to Ubidots server"));
    }

    return isConnectedToServer;
  }

  return false;
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
bool UbiTCP::serverConnected() { return isConnectedToServer; }

/**
 * @brief Handle the proper behavior of the SIM900 before fetching or sending
 * data to Ubidots
 *
 * @return true Everything was fine
 * @return false something went wrong.
 */
bool UbiTCP::_preConnectionChecks() {
  if (sendCommand("AT+CIPSTATUS", "PDP DEACT", 1500)) {
    if (sendCommand("AT+CIPSHUT", "OK")) {
      isInitiatedModule = false;
      isNetworkRegistered = false;
      isJoinedToNetwork = false;
    }
  }

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

void UbiTCP::sendCommand(const char *command, uint16_t timeout) {
  memset(replybuffer, 0, MAX_SERIAL_BUFFER_SIZE);
  while (Sim900->available())  // Clear input buffer
    Sim900->read();

  // if (_debug)
  //   Serial.println(command);

  Sim900->println(command);

  uint8_t idx = 0;
  _timer = millis();

  while (millis() - _timer < timeout) {
    if (Sim900->available()) {
      replybuffer[idx] = Sim900->read();
      idx++;
    }
  }

  // if (_debug)
  //   Serial.print(replybuffer);
}

bool UbiTCP::sendCommandToServer(const char *payload, uint16_t timeout) {
  if (!sendCommand("AT+CIPSEND", ">")) return false;

  memset(replybuffer, 0, MAX_SERIAL_BUFFER_SIZE);

  while (Sim900->available())  // Clear input buffer
    Sim900->read();

  bool gotResponse = false;

  char *modifiedCommand = (char *)malloc(sizeof(char) * strlen(payload) + 15);

  sprintf(modifiedCommand, "%s\r\n\x1A", payload);

  // if (_debug)
  //   Serial.print(modifiedCommand);

  Sim900->print(modifiedCommand);
  free(modifiedCommand);

  uint8_t idx = 0;
  _timer = millis();

  while (millis() - _timer < timeout) {
    if (Sim900->available()) {
      replybuffer[idx] = Sim900->read();
      idx++;
      if (!gotResponse) {
        gotResponse = true;
      }
    }
  }

  // if (_debug)
  //   Serial.print(replybuffer);

  return gotResponse;
}

bool UbiTCP::sendCommand(const char *command, const char *reply, uint16_t timeout) {
  sendCommand(command, timeout);
  return strstr(replybuffer, reply) != NULL;
}