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
const char *APN = "Put_the_APN_here" 
const char *USER = "Put_the_APN_user_here"
const char *PASS = "Put_the_APN_pwd_here"  
const char *TOKEN = "Put_your_Ubidots_token_here" 
const char *DEVICE_LABEL = "Put_your_device_API_label" 
const char *VARIABLE_LABEL_1 = "Put_your_variable_API_label_1"
const char *VARIABLE_LABEL_2 = "Put_your_variable_API_label_2"
const char *VARIABLE_LABEL_3 = "Put_your_variable_API_label_3"

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
  client.setDebug(true); // Set true to make available debug messages
}

void loop() {
  float temperature = analogRead(A0);  // Reading analog pin A0
  float humidity = analogRead(A1);  // Reading analog pin A1
  float pressure = analogRead(A2);  // Reading analog pin A2
  
  client.add(VARIABLE_LABEL_1, temperature);
  client.add(VARIABLE_LABEL_2, humidity);
  client.add(VARIABLE_LABEL_3, pressure);
  
  if(client.send()){
  	Serial.println("Data sent to Ubidots sucessfully!")
  }

  delay(5000);
}
