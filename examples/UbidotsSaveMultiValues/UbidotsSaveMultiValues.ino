#include <Ubidots_Arduino_GPRS.h>
#include <SoftwareSerial.h> 
//Serial Relay - Arduino will patch a 
//serial link between the computer and the GPRS Shield
//at 19200 bps 8-N-1
//Computer is connected to Hardware UART
//GPRS Shield is connected to the Software UART 
#define APN "Your_apn_of_your_SIM_here" 
#define USER "Your_username_here"  // If your apn doesnt have username just put ""
#define PASS "Your_password_here"  // If your apn doesnt have password just put ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define ID "Your_id_here" // Replace it with your Ubidots' variable ID
// You can send 1 to 10 variable at the same time
#define ID1 "Your_id_here" // Replace it with your variable ID
#define ID2 "Your_id_here" // Replace it with your variable ID
#define ID3 "Your_id_here" // Replace it with your variable ID


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

  // To send a value with a context is like: (ID,value,"NAME_CONTEXT:VALUE_CONTEXT","")
  // If you don't want to send any context only put "" like this (ID,value,"","")
  // Example with context:
  // client.add(ID1, value_1, "lat:9.786589", "lng:1.8688797");

  client.add(ID1, value_1, "lat:9.786589", "lng:1.8688797");
  client.add(ID2,value_2, "", "");
  client.add(ID3,value_3, "", "");

  client.sendAll();
}
