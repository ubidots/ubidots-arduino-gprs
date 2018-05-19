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
                                Maria Carlina Hernandez https://github.com/mariacarlinahernandez 
*/

#include <avr/pgmspace.h>
#ifdef ARDUINO_ARCH_AVR
#include <avr/wdt.h>
#else
#define wdt_reset()
#endif
#include "UbidotsArduinoGPRS.h"


/***************************************************************************
CONSTRUCTOR
***************************************************************************/

/**
 * Constructor.
 * Default device_label is GPRS
 */
Ubidots::Ubidots(char* token, char* server) {
    _server = server;
    _token = token;
    _device_label = "GPRS";
    _currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
}

/***************************************************************************
GPRS FUNCTIONS
***************************************************************************/

/**
 * This function is to make the Initialization of the shield
 * @arg apn the apn of your cellular provider
 * @arg apn_user the apn username of your cellular provider
 * @arg apn_pwd the apn password of your cellular provider
 * @return true uppon succes  
 */
void Ubidots::setApn(char* apn, char* apn_user, char* apn_pwd) {

    _apn = apn;
    _apn_user = apn_user;
    _apn_pwd  = apn_pwd;

    int i = 0;

    client->println(F("AT")); //  Test if everything is OK
    if (strstr(readData(2000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT"));
        }
        return false;
    }

    client->println(F("AT+CSQ")); // Signal Quality Report
    if (strstr(readData(2000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CSQ"));
        }
        return false;
    }

    client->println(F("AT+CPIN?")); // Check if SIM is unlocked
    if (strstr(readData(2000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CPIN"));
        }
        return false;
    }

    client->println(F("AT+CREG?")); // Check if SIM is registered
    if (strstr(readData(2000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CREG"));
        }
        return false;
    }

    client->println(F("AT+CGATT?")); // Attach or Detach from GPRS Service      
    if (strstr(readData(2000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CGATT"));
        }
        return false;
    }

    client->print(F("AT+CSTT=\"")); // Start the task
    client->print(apn);
    client->print(F("\",\""));
    client->print(apn_user);
    client->print(F("\",\""));
    client->print(apn_pwd);
    client->println("\"");
    if (strstr(readData(4000), "OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CSTT=APN,USER,PWD. "));
        }
        return false;
    } 

    client->println(F("AT+CIICR")); // Bring up the Wireless
    if (strstr(readData(4000), "OK")==NULL) {
        if (_debug) {
            Serial.println("Error with AT+CIICR. Wireless DOWN");
        }
        return false;
    }

    client->println(F("AT+CIFSR")); // Get the local IP address
    if (strstr(readData(4000), "OK")!=NULL) {
        if (_debug) {
            Serial.println("Error with AT+CIFSR. No IP obtained");
        }
        return false;
    }
    return true;
}

bool Ubidots::manageData(char *allData){

    int data = strlen(allData);
    char buffer_size[3];
    sprintf(buffer_size, "%d", data);

    client->println(F("AT+CIPSHUT")); // Reset IP session
    if (strstr(readData(4000),"SHUT OK")==NULL) {
        if  (_debug) {
            Serial.println(F("Error with AT+CIPSHUT"));
        }
        _currentValue = 0;
        return false;
    }

    client->println(F("AT+CIPSTATUS")); // Check IP
    if (strstr(readData(4000),"STATE: IP INITIAL")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSTATUS"));
        }
        _currentValue = 0;
        return false;
    }

    client->println(F("AT+CIPMUX=0")); // Setting up single connection
    if (strstr(readData(4000),"OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPMUX"));
        }
        _currentValue = 0;
        return false;
    }

    client->print(F("AT+CIPSTART=\"TCP\",\"")); // Start TCP connection
    client->print(_server);
    client->print(F("\",\""));
    client->print(PORT);
    client->println(F("\""));
    if (strstr(readData(4000),"CONNECT OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSTART"));
        }
        _currentValue = 0;
        return false;
    }

    client->print(F("AT+CIPSEND=")); // Request initiation of data sending 
    client->println(buffer_size);
    if (strstr(readData(4000),">")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSEND"));
        }
        _currentValue = 0;
        return false;
    }
    _currentValue = 0;
    return true;
}


/***************************************************************************
FUNCTIONS TO RETRIEVE DATA
***************************************************************************/

/** 
 * This function is to get value from the Ubidots API with the device label
 * and variable label
 * @arg device_label is the label of the device
 * @arg variable_label is the label of the variable
 * @return num the data that you get from the Ubidots API, if any error occurs
    the function returns ERRORs
 */
float Ubidots::getValueWithDevice(char* device_label, char* variable_label) {
    
    char* serverResponse; 
    char* response;
    char* lectura = (char *) malloc(sizeof(char) * 100);
    float num;


    char* allData = (char *) malloc(sizeof(char) * 700);
    sprintf(allData, "%s/%s|LV|%s|%s:%s|end", USER_AGENT, VERSION, _token, device_label, variable_label);
    
    if (manageData(allData)) {
        client->write(allData); // Send request
        char* resp = readData(4000);      
        if (strstr(resp,"ERROR")!=NULL) {
            if (_debug) {
                Serial.println(F("Error getting variables. Please verify the device label and variable label"));
            }
            _currentValue = 0;
            return ERROR_VALUE;
        } 
        response = strtok(resp, "|");
        serverResponse = response;
        while (response!=NULL) {
            printf("%s\n", response);
            response = strtok(NULL, "|");
            if (response != NULL) {
                serverResponse = response;
            }
        }
        num = atof(serverResponse);
        if (_debug) {
            Serial.print("Value obtained: ");
            Serial.println(num);
        }
          
        client->println(F("AT+CIPCLOSE")); // Close the TCP connection 
        if (strstr(readData(4000),"CLOSE OK")==NULL) {
            if (_debug) {
                Serial.println(F("Error with AT+CIPCLOSE"));
            }
            _currentValue = 0;
            return false;
        }

        client->println(F("AT+CIPSHUT")); // Reset IP session
        if (strstr(readData(4000),"SHUT OK")==NULL) {
            if (_debug) {
                Serial.println(F("Error with AT+CIPSHUT"));
            }
            _currentValue = 0;
            return false;
        }
        free(allData);
        _currentValue = 0;
        return num;
        }
}

/***************************************************************************
FUNCTIONS TO SEND DATA
***************************************************************************/

/**
 * Add a value of variable to save
 * @arg variable_label variable label or name to save in a struct
 * @arg value variable value to save in a struct
 * @arg ctext [optional] is the context that you will save, default
 * @arg timestamp_val [optional] is the timestamp for the actual value
 * is NULL
 */
void Ubidots::add(char *variable_label, double value) {
  return add(variable_label, value, NULL, NULL);
}


void Ubidots::add(char *variable_label, double value, char *ctext) {
  return add(variable_label, value, ctext, NULL);
}


void Ubidots::add(char *variable_label, double value, char *ctext, long unsigned timestamp_val) {
  (val+_currentValue)->varName = variable_label;
  (val+_currentValue)->varValue = value;
  (val+_currentValue)->contextOne = ctext;
  (val+_currentValue)->timestamp_val = timestamp_val;
  _currentValue++;
  if (_currentValue > MAX_VALUES) {
        Serial.println(F("You are sending more than the maximum of consecutive variables"));
        _currentValue = MAX_VALUES;
  }
}

/**
 * This function is to set the name of your device to visualize,
 * if you don't call this method the name by default will be 'GPRS'
 * @arg device_name is the name to display in Ubidots, avoid to use special
 * characters or blank spaces
 * @return true uppon succes
 */
void Ubidots::setDeviceName(char* device_name) {
    _device_name = device_name;
}

/**
 * This function is to set your device label, the device
 * label is the unique device identifier in Ubidots.
 * @arg device_label is the device label, avoid to use special
 * characters or blank spaces
 * @return true uppon succes
 */
void Ubidots::setDeviceLabel(char* device_label) {
    _device_label = device_label;
}

/**
 * Send all data of all variables that you saved
 * @arg timestamp_global [optional] is the timestamp for all the variables added
 * using add() method, if a timestamp_val was declared on the add() method, Ubidots
 * will take as timestamp for the val the timestamp_val instead of the timestamp_global
 * @reutrn true upon success, false upon error.
 */
bool Ubidots::sendAll(){
    return sendAll(NULL);
}

bool Ubidots::sendAll(unsigned long timestamp_global) {
     
    int i;
    char* allData = (char *) malloc(sizeof(char) * 700);
    free(allData); //Find error on Arduino mini pro. Fixed with this instance, maybe variable overflow. More details in description
    char str_values[10];
    
    if (timestamp_global!= NULL) {
        if (_device_label == "GPRS") {
            sprintf(allData, "%s/%s|POST|%s|%s@%lu%s=>", USER_AGENT, VERSION, _token, _device_label, timestamp_global, "000");
        } else {
            sprintf(allData, "%s/%s|POST|%s|%s:%s@%lu%s=>", USER_AGENT, VERSION, _token, _device_label, _device_name, timestamp_global, "000");
        }
    } else {
        if (_device_label == "GPRS") {
            sprintf(allData, "%s/%s|POST|%s|%s=>", USER_AGENT, VERSION, _token, _device_label);
        } else {
            sprintf(allData, "%s/%s|POST|%s|%s:%s=>", USER_AGENT, VERSION, _token, _device_label, _device_name);
        }
    }

    for (i = 0; i < _currentValue; ) {
        dtostrf(((val+i)->varValue), 4, 4, str_values);
        sprintf(allData, "%s%s:%s", allData, (val + i)->varName, str_values);
        if ((val + i)->timestamp_val != NULL) {
            sprintf(allData, "%s@%lu%s", allData, (val + i)->timestamp_val, "000");
        }
        if ((val + i)->contextOne != NULL) {
            sprintf(allData, "%s$%s", allData, (val + i)->contextOne);
        }
        i++;
        if (i < _currentValue) {
            sprintf(allData, "%s,", allData);
        }
    }

    sprintf(allData, "%s|end", allData);

    if (_debug) {
        Serial.println(allData);
    }

    manageData(allData);

    client->write(allData); // Send request
    if (strstr(readData(4000),"SEND OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error sending variables "));
        }
        _currentValue = 0;
        return false;
    }

    client->println(F("AT+CIPCLOSE")); // Close the TCP connection 
    if (strstr(readData(4000),"CLOSE OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPCLOSE"));
        }
        _currentValue = 0;
        return false;
    }

    client->println(F("AT+CIPSHUT")); // Reset IP session
    if (strstr(readData(4000),"SHUT OK")==NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSHUT"));
        }
        _currentValue = 0;
        return false;
    }
    free(allData);
    _currentValue = 0;
    return true;
}

/***************************************************************************
AUXILIAR FUNCTIONS
***************************************************************************/

/**
 * This function is to initialize the serial communication 
 */
bool Ubidots::init(Stream &port) {
    client = &port;
    powerUpOrDown();
    return true;
}

/** 
 * This function is to power up or down GPRS Shield
 */
void Ubidots::powerUpOrDown() {
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);
  readData(4000);
}

/**
 * Turns on or off debug messages
 * @debug is a bool flag to activate or deactivate messages
 */
void Ubidots::setDebug(bool debug) {
     _debug = debug;
}

/** 
 * This function is to read the data from GPRS pins. This function is from Adafruit_FONA library
 * @arg timeout, time to delay until the data is transmited
 * @return replybuffer the data of the GPRS
 */
char* Ubidots::readData(uint16_t timeout) {
  uint16_t replyidx = 0;
  char replybuffer[500];
  while (timeout--) {
    if (replyidx >= 500) {
      break;
    }
    while (client->available()) {
      char c =  client->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;
      }
      replybuffer[replyidx] = c;
      replyidx++;
    }

    if (timeout == 0) {
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = '\0';  // null term
  if (_debug) {
      Serial.println("Response of GPRS:");
      Serial.println(replybuffer);
  }

  while (client->available()) {
    client->read();
  }
  if (strstr(replybuffer,"NORMAL POWER DOWN")!=NULL) {
    powerUpOrDown();
  }
  return replybuffer;
}
