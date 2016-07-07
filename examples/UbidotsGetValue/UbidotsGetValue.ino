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
#define DATA_SOURCE_IDENTIFIER "Your_data_source_identifier_here"
#define VARIABLE_IDENTIFIER "Your_variable_identifier_here"

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
  float value = client.getValueWithDatasource(DATA_SOURCE_IDENTIFIER, VARIABLE_IDENTIFIER));
  Serial.println(value);
  delay(1000);
  Serial.println(value);
}