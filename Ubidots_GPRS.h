#ifndef __Ubidots_GPRS_H_
#define __Ubidots_GPRS_H_
#define DEBUG_UBIDOTS

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#define USER_AGENT  "Ubidots/v1 ArduinoGPRS/1.0"
#define BAUDRATE 19200

typedef struct Value {
  char  *id;
  char  *context_1;
  char  *context_2;
  float value_id;
} Value;

class Ubidots{
	private:
		SoftwareSerial _client = SoftwareSerial(7, 8);		
		bool http_init();
	    char* _token;
	    bool http_term();
	    char* read_data(uint16_t timeout);
    	void flush_input();
    	uint8_t n;
    	uint8_t summ;
    	Value * val;
    	

	public:
		Ubidots(char* token);
		void power_up_or_down();
		bool set_apn(char* apn, char* user, char* pwd);
		bool save_value(double value, char* id);
		float get_value(char* id);
		int free_ram();
		void add(char *variable_id, double value, char *context1, char *context2);
		bool send_all();		
};
#endif