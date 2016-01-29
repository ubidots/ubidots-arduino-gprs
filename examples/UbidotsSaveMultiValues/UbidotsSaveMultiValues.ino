#include <Ubidots_Arduino_GPRS.h>
#include <SoftwareSerial.h> 
//Serial Relay - Arduino will patch a 
//serial link between the computer and the GPRS Shield
//at 19200 bps 8-N-1
//Computer is connected to Hardware UART
//GPRS Shield is connected to the Software UART 

#define APN "Your_apn_of_your_SIM_here" 
#define USER "Your_username_here"  // if your apn doesnt have username just put ""
#define PASS "Your_password_here"  // if your apn doesnt have password just put ""
#define TOKEN "Your_token_here"  // Remplace it with your token
// You can send 1 to 10 variable at the same time
#define ID1 "Your_id_here" // Remplace it with your variable ID
#define ID2 "Your_id_here" // Remplace it with your variable ID
#define ID3 "Your_id_here" // Remplace it with your variable ID
#define ID4 "Your_id_here" // Remplace it with your variable ID
#define ID5 "Your_id_here" // Remplace it with your variable ID
#define ID6 "Your_id_here" // Remplace it with your variable ID

Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(19200);             // the Serial port of Arduino baud rate.  
  client.powerUpOrDown();
  client.setApn(APN,USER,PASS);
}

void loop() {
  float value_1 = analogRead(A0);
  float value_2 = analogRead(A1);
  float value_3 = analogRead(A2);
  float value_4 = analogRead(A3);
  float value_5 = analogRead(A4);
  float value_6 = analogRead(A5);
  // To send a value with a context is like: (ID,value,"NAME_CONTEXT:VALUE_CONTEXT","NAME_CONTEXT_2:VALUE_CONTEXT_2")
  // If you don't want to send any context only put "" like this (ID,value,"","")
  // Example:
  // client.add(ID1, value_1, "Color:Red", "Sensor:Temp");

  client.add(ID1,value_1, "name_context_1:value_context_1", "name_context_2:value_context_2"); 
  client.add(ID2,value_2, "name_context_1:value_context_1", "name_context_2:value_context_2");
  client.add(ID3,value_3, "name_context_1:value_context_1", "name_context_2:value_context_2");
  client.add(ID4,value_4, "name_context_1:value_context_1", "name_context_2:value_context_2");
  client.add(ID5,value_5, "name_context_1:value_context_1", "name_context_2:value_context_2");
  client.add(ID6,value_6, "name_context_1:value_context_1", "name_context_2:value_context_2");
  client.sendAll();
}
