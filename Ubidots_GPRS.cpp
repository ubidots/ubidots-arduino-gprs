#include "Ubidots_GPRS.h"
#include <SoftwareSerial.h>

/**
 * Constructor.
 */
Ubidots::Ubidots(char* token){
    _token = token;
    _client.begin(BAUDRATE);
    maxValues = 10;
    currentValue = 0;
    val = (Value *)malloc(maxValues*sizeof(Value));
}
/** 
 * This function is to power up or down GPRS Shield
 */
void Ubidots::powerUpOrDown(){
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
 * This function is to read the data from GPRS pins. This function is from Adafruit_FONA library
 * @arg timeout, time to delay until the data is transmited
 * @return replybuffer the data of the GPRS
 */
char* Ubidots::readData(uint16_t timeout){
  uint16_t replyidx = 0;
  char replybuffer[500];
  while (timeout--) {
    if (replyidx >= 500) {
      break;
    }
    while(_client.available()) {
      char c =  _client.read();
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
#ifdef DEBUG_UBIDOTS
  Serial.println("Response of GPRS:");
  Serial.println(replybuffer);
#endif
  while(_client.available()){
    _client.read();
  }
  if(strstr(replybuffer,"NORMAL POWER DOWN")!=NULL){
    powerUpOrDown();
  }
  return replybuffer;
}
/** 
 * This function is to set the APN, USER and PASSWORD
 * @arg apn the APN of your mobile
 * @arg user the USER of the APN
 * @arg pwd the PASSWORD of the APN
 * @return true upon success
 */
bool Ubidots::setApn(char* apn, char* user, char* pwd){
    _client.println(F("AT+CSQ"));
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CSQ"));
#endif
        return false;
    }
    _client.println(F("AT+CGATT?"));    
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CGATT"));
#endif
        return false;
    }
    _client.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));        
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR CONTYPE"));
#endif
        return false;
    }
    _client.print(F("AT+SAPBR=3,1,\"APN\",\""));
    _client.print(apn);
    _client.println(F("\""));   
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR APN"));
#endif
        return false;
    }   
    _client.print(F("AT+SAPBR=3,1,\"USER\",\""));
    _client.print(user);
    _client.println(F("\""));       
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR USER"));
#endif
        return false;
    }   
    _client.print(F("AT+SAPBR=3,1,\"PWD\",\""));
    _client.print(pwd);
    _client.println("\"");              
    if(strstr(readData(3000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR PASSWORD"));
#endif
        return false;
    }
    _client.println(F("AT+SAPBR=1,1"));     
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=1,1 Connection ip"));
#endif
        return false;
    }
    _client.println(F("AT+SAPBR=2,1"));     
    if(strstr(readData(4000),"+SAPBR:")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=2,1 no IP to show"));
#endif
        return false;
    }
}
/** 
 * This function is to save value to Ubidots API
 * @arg value the value that you will send
 * @arg id the id of the variable where you will save in the Ubidots API
 * @return true upon success
 */
bool Ubidots::saveValue(double value, char* id){
    char data[25];
    char val[10];
    dtostrf(value,7, 3, val);
    httpTerm();
    sprintf(data,"{\"value\":%s}", val);
    Serial.println(data);
    httpInit();
    Serial.println(_token);
    _client.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    _client.print(id);
    _client.print(F("/values?token="));
    _client.print(_token);
    _client.println("\"");
    if(strstr(readData(4000),"OK")==NULL){
        Serial.println(F("Error with AT+HTTPARA URL"));     
        httpTerm();
        return false;
    }
    _client.println(F("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
    if(strstr(readData(4000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPARA CONTENT"));
#endif
        httpTerm();
        return false;
    }
    _client.print(F("AT+HTTPDATA="));
    _client.print(strlen(data));
    _client.print(F(","));
    _client.println(120000);
    if(strstr(readData(2000),"DOWNLOAD")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPDATA"));
#endif
        httpTerm();
        return false;
    }
    _client.write(data, strlen(data));
    if(strstr(readData(2000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending data and data length"));
#endif
        httpTerm();
        return false;
    }
    _client.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method
    delay(5000);
    if(strstr(readData(4000),"+HTTPACTION:1,201")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPACTION=1"));
        Serial.println(F("Status = 603 is DNS Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 601 Network Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 400 Bad Request, Your URL is wrong"));
        Serial.println(F("Status = 402 Payment Required, You exceed your dots in Ubidots"));
#endif
        httpTerm();
        return false;       
    }
    else{
        _client.println(F("AT+HTTPREAD"));
        if(strstr(readData(4000),"+HTTPREAD:")==NULL){
#ifdef DEBUG_UBIDOTS
            Serial.println(F("Error with AT+HTTPREAD. Closing HTTP Client"));
#endif
            httpTerm();
            return false;
        }
    }
    httpTerm();
    return true;
    
}
/** 
 * This function is to get value from the Ubidots API
 * @arg id the id where you will get the data
 * @return num the data that you get from the Ubidots API
 */
float Ubidots::getValue(char* id){
    float num;
    String raw;
    uint8_t bodyPosinit;
    uint8_t bodyPosend;
    flushInput();
    httpTerm();
    httpInit();
    _client.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    _client.print(id);
    _client.print(F("/values?token="));
    _client.print(_token); //&page_size=1
    _client.println(F("&page_size=1\""));
    if(strstr(readData(1000),"OK")==NULL){
        httpTerm();
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPPARA URL. Closing HTTP Client"));
#endif      
        return 1;
    }
    _client.println(F("AT+HTTPACTION=0"));  //HTTPACTION=0 is GET method
    if(strstr(readData(4000),"+HTTPACTION:0,200")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Status = 603 is DNS Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 601 Network Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 400 Bad Request, Your URL is wrong"));
        Serial.println(F("Status = 402 Payment Required, You exceed your dots in Ubidots"));
#endif
        httpTerm();
        return 1; 
    }
    else{
        _client.println(F("AT+HTTPREAD"));
        char* reply = readData(3000);
        if(strstr(reply,"+HTTPREAD:")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPREAD. Closing HTTP Client"));
#endif
            httpTerm();
            return 1;
        }
        char* pch = strstr(reply,"\"value\":");
#ifdef DEBUG_UBIDOTS
        String raw(pch);
#endif
        int bodyPosinit =9+ raw.indexOf("\"value\":");
        int bodyPosend = raw.indexOf(", \"timestamp\"");
        raw.substring(bodyPosinit,bodyPosend).toCharArray(reply, 10);
        num = atof(reply);      
        httpTerm();
        return num;
    }

}
bool Ubidots::httpInit(){
    _client.println(F("AT+HTTPINIT"));
    if(strstr(readData(1000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPINIT. Reset the Arduino, will you?"));
#endif
        return false;
    }
    _client.println(F("AT+HTTPPARA=\"CID\",1"));
    if(strstr(readData(1000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPARA CID. Reset the Arduino, will you?"));
#endif
        return false;
    }
    _client.print(F("AT+HTTPPARA=\"UA\","));
    _client.print(USER_AGENT);
    _client.println(F("\""));
    if(strstr(readData(1000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPARA USER_AGENT. Reset the Arduino, will you?"));
#endif
        return false;
    }
}
bool Ubidots::httpTerm(){
    _client.println(F("AT+HTTPTERM"));
    if(strstr(readData(1000),"OK")==NULL){
#ifdef DEBUG_UBIDOTS
        Serial.println(F("No HTTP to close"));
#endif
        return false;
    }
    return true;
}
/** 
 * This function is to flush the input in the serial terminal. This function is from Adafruit_FONA library
 */
void Ubidots::flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(Serial.available()) {
            Serial.read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        delay(1);
    }
}
/**
 * Add a value of variable to save
 * @arg variable_id variable id to save in a struct
 * @arg value variable value to save in a struct
 * @arg context1 context name : context value to save in a struct
 * @arg context2 context name : context value to save in a struct
 */
void Ubidots::add(char *variable_id, double value, char *context1, char *context2){
  (val+currentValue)->id = variable_id;
  (val+currentValue)->context_1 = context1;
  (val+currentValue)->context_2 = context2;
  (val+currentValue)->value_id = value;
  currentValue++;
}


/**
 * Send all data of all variables that you saved
 * @reutrn true upon success, false upon error.
 */
bool Ubidots::sendAll(){
    int i;
    int len = 0;
    char vals[10];
    httpTerm();
    // Next for is to calculate the lenght of the data that you will send
    for(i=0;i<currentValue;){
        len = len + strlen((val+i)->id) + 1 + 10 + 1 + strlen((val+i)->context_1)+ 1 + strlen((val+i)->context_2)+1;
        i++;
    }
    httpInit();
    _client.print(F("AT+HTTPPARA=\"URL\",\"translate.ubidots.com:9080/arduino/gprs/?token="));
    _client.print(_token);
    _client.println("\"");
    if(strstr(readData(3000),"OK")==NULL){
        Serial.println(F("error AT+HTTPARA"));      
        httpTerm();
        currentValue = 0;
        return false;
    }
    _client.print(F("AT+HTTPDATA="));
    _client.print(len);
    _client.print(F(","));
    _client.println(120000);
    if(strstr(readData(2000),"DOWNLOAD")==NULL){
        Serial.println(F("error"));
        currentValue = 0;
        httpTerm();
        return false;
    }
    // Next for is to send all data from the struct
    for(i=0;i<currentValue;){
        dtostrf((val+i)->value_id,10, 5, vals);     
        _client.print((val+i)->id);
        _client.print(",");
        _client.print(vals);
        _client.print(",");
        _client.print((val+i)->context_1);
        _client.print(",");
        _client.print((val+i)->context_2);
        _client.print("&"); 
        i++;
    }
    _client.println();
    if(strstr(readData(2000),"OK")==NULL){
        Serial.println(F("error"));
        httpTerm();
        currentValue = 0;
        return false;
    }
    _client.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method
    if(strstr(readData(4000),"+HTTPACTION:1,201")==NULL){
        httpTerm();
        currentValue = 0;
        return false;       
    }
    else{
        _client.println(F("AT+HTTPREAD"));
        if(strstr(readData(4000),"+HTTPREAD:")==NULL){
            Serial.println(F("error"));
            httpTerm();
            currentValue = 0;
            return false;
        }
    }
    httpTerm();
    currentValue = 0;
    return true;
}