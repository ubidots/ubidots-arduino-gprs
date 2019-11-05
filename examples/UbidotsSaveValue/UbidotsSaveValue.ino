// This example retrieves last value of a variable from the Ubidots API
// using TCP protocol.

/****************************************
 * Include Libraries
 ****************************************/
#include <Ubidots.h>

/****************************************
 * Define Constants
 ****************************************/
// Your GPRS credentials, if any
#define APN             "Put_the_APN_here" 
#define USER            "Put_the_APN_user_here"
#define PASS            "Put_the_APN_pwd_here"  
#define TOKEN           "Put_your_Ubidots_token_here" 
#define VARIABLE_LABEL  "Put_your_variable_API_label" 

Ubidots client(TOKEN,APN,USER,PASS);


/****************************************
 * Auxiliar Functions
 ****************************************/

// Put here your auxiliar functions

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  Serial.begin(115200);
  client.setDebug(false);
}

void loop() {
  float value = analogRead(A0);  // Reading analog pin A0
  client.add(VARIABLE_LABEL, value);  
  client.send();
}
