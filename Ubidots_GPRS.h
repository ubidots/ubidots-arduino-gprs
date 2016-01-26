#ifndef __Ubidots_GPRS_H_
#define __Ubidots_GPRS_H_
#define DEBUG_UBIDOTS

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#define USER_AGENT  "Ubidots/v1 GPRS/1.0"
#define BAUDRATE 19200




class Ubidots{
	private:
		SoftwareSerial _client = SoftwareSerial(7, 8);		
		bool http_init();
	    char* _token;
	    bool http_term();
	    char* readData(uint16_t timeout);
    	void flushInput();
    	

	public:
		Ubidots(char* token);
		void powerUpOrDown();
		bool setApn(char* apn, char* user, char* pwd);
		bool save_value(double value, char* id);
		float get_value(char* id);
		int freeRam();

		
};
#endif