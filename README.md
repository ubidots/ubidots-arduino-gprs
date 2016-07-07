# Arduino-GPRS v2.0

The GPRS Shield is based on the SIM900 module from SIMCOM. It is compatible with all boards that have the same form factor (and pinout) as a standard Arduino Board. The GPRS Shield is configured and controlled via its UART using simple AT commands. Based on the SIM900 module from SIMCOM, the GPRS Shield is like a cell phone. 
Here you will learn how to interact with the Ubidots API using an Arduino GPRS shield and a Ubidots library.

## Requirements

* [Arduino Uno](http://arduino.cc/en/Main/ArduinoBoardUno)
* [GPRS shield SIM900](http://www.seeedstudio.com/depot/GPRS-Shield-V20-p-1379.html)
* [Arduino IDE 1.6.0 or higher](https://www.arduino.cc/en/Main/Software)
* [Ubidots Arduino GPRS library](https://github.com/ubidots/ubidots-arduino-gprs/archive/1.0.0.zip)
* An active SIM card with a data plan 

## Setup

1. Download the Ubidots_Arduino_GPRS.h library [here](https://github.com/ubidots/ubidots-arduino-gprs/archive/1.0.0.zip)
2. Go to the Arduino IDE, click on **Sketch -> Include Library -> Add .ZIP Library**
3. Select the .ZIP file of Ubidots_Arduino_GPRS.h and then "Accept" or "Choose"
4. Close the Arduino IDE and open it again.

<aside class="alert">
Before running the examples, make sure you have an active data plan. You will also need your mobile operator’s APN settings (APN, USER, PASSWORD). You should be able to easily find these settings in Google or in your operator’s website.
</aside>

<aside class="warning">
This library uses the digital PIN "JP" of the SIM900 (PIN 9 in the Arduino) to power on/off the GPRS shield. Make sure the JP pad is soldered. The pad of the JP pin can be found next to the ISP port of the GPRS board.
</aside>

## Send values to Ubidots 

To send multiple values to Ubidots, go to **Sketch -> Examples -> Ubidots Arduino GPRS library** and select the "saveValues" example. 
Put your Ubidots token and variable ID's where indicated, as well as the APN settings.
Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

```c++
#include <UbidotsArduinoGPRS.h>
#include <SoftwareSerial.h> 
//Serial Relay - Arduino will patch a 
//serial link between the computer and the GPRS Shield
//at 19200 bps 8-N-1
//Computer is connected to Hardware UART
//GPRS Shield is connected to the Software UART 
#define APN "myapn.com" 
#define USER ""  // If your apn doesnt have username just put ""
#define PASS ""  // If your apn doesnt have password just put ""
#define TOKEN "XXXxxxxXXXxx"  // Replace it with your Ubidots token
#define VARIABLE_NAME_ONE "Temperature"
#define VARIABLE_NAME_TWO "Humidity"
#define VARIABLE_NAME_THREE "Heat Index"


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

  // To send geolocation in a variable, use this format:
  client.add(VARIABLE_NAME_ONE, value_1, "lat:9.92323$lng:1.12323");
  // lng is longitude, and lat is latitude, split them with "$"
  client.add(VARIABLE_NAME_TWO, value_2);
  client.add(VARIABLE_NAME_THREE, value_3);
  client.sendAll();
}
```


## Get one value from Ubidots

To get the last value of a variable from Ubidots, go to **Sketch -> Examples -> Ubidots Arduino GPRS library** and select the "getValue" example. 
Put your Ubidots token and variable ID where indicated, as well as the APN settings.
Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

```c++
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
```