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
Ubidots client(TOKEN);

void setup() {
  Serial.begin(19200);             // the Serial port of Arduino baud rate.  
  client.powerUpOrDown();
  client.setApn(APN,USER,PASS);
}

void loop() {
  float value = analogRead(A0);  // Reading analog pin A0
  client.saveValue(value,ID);  
  delay(1000);
}
