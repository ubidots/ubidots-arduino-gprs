#include <UbidotsArduinoGPRS.h>
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
#define VARIABLE_NAME_ONE "Your_variable_name_here"
#define VARIABLE_NAME_TWO "Your_variable_name_here"
#define VARIABLE_NAME_THREE "Your_variable_name_here"


Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  GPRSSerial->begin(19200);
  if (! client.init(*GPRSSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  while (!client.setApn(APN,USER,PASS));
}

void loop() {
  float value_1 = analogRead(A0);
  float value_2 = analogRead(A1);
  float value_3 = analogRead(A2);

  // To send geolocation of the variable put it like this:
  client.add(VARIABLE_NAME_ONE, value_1, "lat=9.92323$lng=1.12323");
  // lng is longitude, and lat is latitude, split it with "$"
  client.add(VARIABLE_NAME_TWO, value_2);
  client.add(VARIABLE_NAME_THREE, value_3);
  client.sendAll();
}