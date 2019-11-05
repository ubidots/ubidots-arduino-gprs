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
#define DEVICE_LABEL    "Put_your_device_API_label" 
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
  float value = client.get(DEVICE_LABEL, VARIABLE_LABEL);
  
  if(value!=ERROR_VALUE){
    Serial.print("Getting variable value: ");
    Serial.println(value);
  }

  delay(5000);
}
