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
const char *APN = "Put_the_APN_here";
const char *USER = "Put_the_APN_user_here";
const char *PASS = "Put_the_APN_pwd_here";
const char *TOKEN = "Put_your_Ubidots_token_here";
const char *DEVICE_LABEL = "Put_your_device_API_label";
const char *VARIABLE_LABEL = "Put_your_variable_API_label"; 

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
  client.setDebug(true);// Set true to make available debug messages
}

void loop() {
  float value = client.get(DEVICE_LABEL, VARIABLE_LABEL);
  
  if(value!=ERROR_VALUE){
    Serial.print("Getting variable value: ");
    Serial.println(value);
  }

  delay(5000);
}
