#include "Ubidots_GPRS.h"
#include <SoftwareSerial.h>


Ubidots::Ubidots(char* token){
	_token = token;
	_client.begin(BAUDRATE);
}
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
char* Ubidots::readData(uint16_t timeout){
  uint16_t replyidx = 0;
  char replybuffer[500];
  while (timeout--) {
    if (replyidx >= 500) {
      //Serial.println(F("SPACE"));
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
      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);
      replyidx++;
    }

    if (timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = '\0';  // null term
  Serial.println(replybuffer);
  while(_client.available()){
  	_client.read();
  }
  return replybuffer;
}
bool Ubidots::setApn(char* apn, char* user, char* pwd){
	_client.println(F("AT+CSQ"));
	if(strstr(readData(2000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.println(F("AT+CGATT?"));	
	if(strstr(readData(2000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));		
	if(strstr(readData(3000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.print(F("AT+SAPBR=3,1,\"APN\",\""));
	_client.print(apn);
	_client.println(F("\""));	
	if(strstr(readData(3000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}	
	_client.print(F("AT+SAPBR=3,1,\"USER\",\""));
	_client.print(user);
	_client.println(F("\""));		
	if(strstr(readData(3000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}	
	_client.print(F("AT+SAPBR=3,1,\"PWD\",\""));
	_client.print(pwd);
	_client.println("\"");				
	if(strstr(readData(3000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.println(F("AT+SAPBR=1,1"));		
	if(strstr(readData(4000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.println(F("AT+SAPBR=2,1"));		
	if(strstr(readData(4000),"+SAPBR:")==NULL){
		Serial.println(F("error"));
		return false;
	}
}
bool Ubidots::save_value(double value, char* id){
    char data[25];
    char val[10];
    dtostrf(value,7, 3, val);
    http_term();
	sprintf(data,"{\"value\":%s}", val);
	Serial.println(data);
	http_init();
	Serial.println(_token);
    _client.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    _client.print(id);
    _client.print(F("/values?token="));
    _client.print(_token);
	_client.println("\"");
	if(strstr(readData(4000),"OK")==NULL){
		Serial.println(F("error AT+HTTPARA"));		
		http_term();
		return false;
	}
	_client.println(F("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
	if(strstr(readData(4000),"OK")==NULL){
		Serial.println(F("error"));
		http_term();
		return false;
	}
	_client.print(F("AT+HTTPDATA="));
	_client.print(strlen(data));
	_client.print(F(","));
  	_client.println(120000);
	if(strstr(readData(2000),"DOWNLOAD")==NULL){
		Serial.println(F("error"));
		http_term();
		return false;
	}
	_client.write(data, strlen(data));
	if(strstr(readData(2000),"OK")==NULL){
		Serial.println(F("error"));
		http_term();
		return false;
	}
	_client.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method
	delay(5000);
	if(strstr(readData(4000),"+HTTPACTION:1,201")==NULL){
		http_term();
		return false;		
	}
	else{
		_client.println(F("AT+HTTPREAD"));
		if(strstr(readData(4000),"+HTTPREAD:")==NULL){
			Serial.println(F("error"));
			http_term();
			return false;
		}
	}
	http_term();
	return true;
	
}
float Ubidots::get_value(char* id){
	float num;
	Serial.println(freeRam());
	String raw;
	uint8_t bodyPosinit;
	uint8_t bodyPosend;
	flushInput();
	http_term();
	http_init();
	Serial.println(freeRam());
	_client.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    _client.print(id);
    _client.print(F("/values?token="));
    _client.print(_token); //&page_size=1
	_client.println(F("&page_size=1\""));
	if(strstr(readData(1000),"OK")==NULL){
		http_term();
		Serial.println(F("error"));		
		return 1;
	}
	_client.println(F("AT+HTTPACTION=0"));  //HTTPACTION=0 is GET method
	Serial.println(freeRam());
	if(strstr(readData(4000),"+HTTPACTION:0,200")==NULL){
		http_term();
		return 1; 
	}
	else{
		Serial.println(freeRam());
		_client.println(F("AT+HTTPREAD"));
		Serial.println(freeRam());
		char* reply = readData(3000);
		if(strstr(reply,"+HTTPREAD:")==NULL){
			Serial.println(F("error"));
			http_term();
			return 1;
		}
		char* pch = strstr(reply,"\"value\":");
	    String raw(pch);
	    int bodyPosinit =9+ raw.indexOf("\"value\":");
	    int bodyPosend = raw.indexOf(", \"timestamp\"");
	    raw.substring(bodyPosinit,bodyPosend).toCharArray(reply, 10);
	    num = atof(reply);
		
		http_term();
		return num;
	}

}
bool Ubidots::http_init(){
	_client.println(F("AT+HTTPINIT"));
	if(strstr(readData(1000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.println(F("AT+HTTPPARA=\"CID\",1"));
	if(strstr(readData(1000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	_client.print(F("AT+HTTPPARA=\"UA\","));
	_client.print(USER_AGENT);
	_client.println(F("\""));
	if(strstr(readData(1000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	Serial.println(freeRam ());
}
bool Ubidots::http_term(){
	_client.println(F("AT+HTTPTERM"));
	if(strstr(readData(1000),"OK")==NULL){
		Serial.println(F("error"));
		return false;
	}
	return true;
}
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
int Ubidots::freeRam (){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}