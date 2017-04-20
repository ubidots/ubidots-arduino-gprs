# Arduino-GPRS v2.0

The GPRS Shield is based on the SIM900 module from SIMCOM. It is compatible with all boards that have the same form factor (and pinout) as a standard Arduino Board. The GPRS Shield is configured and controlled via its UART using simple AT commands. Based on the SIM900 module from SIMCOM, the GPRS Shield is like a cell phone. 
Here you will learn how to interact with the Ubidots API using an Arduino GPRS shield and a Ubidots library.

## Requirements

* [Arduino Uno](http://arduino.cc/en/Main/ArduinoBoardUno)
* [GPRS shield SIM900](http://www.seeedstudio.com/depot/GPRS-Shield-V20-p-1379.html)
* [Arduino IDE 1.6.0 or higher](https://www.arduino.cc/en/Main/Software)
* [Ubidots Arduino GPRS library](https://github.com/ubidots/ubidots-arduino-gprs/archive/master.zip)
* An active SIM card with a data plan 

## Setup

1. Download the Ubidots_Arduino_GPRS.h library [here](https://github.com/ubidots/ubidots-arduino-gprs/archive/master.zip)
2. Go to the Arduino IDE, click on **Sketch -> Include Library -> Add .ZIP Library**
3. Select the .ZIP file of Ubidots_Arduino_GPRS.h and then "Accept" or "Choose"
4. Close the Arduino IDE and open it again.

Before running the examples, make sure you have an active data plan. You will also need your mobile operator’s APN settings (APN, USER, PASSWORD). You should be able to easily find these settings in Google or in your operator’s website.

This library uses the digital PIN "JP" of the SIM900 (PIN 9 in the Arduino) to power on/off the GPRS shield. Make sure the JP pad is soldered. The pad of the JP pin can be found next to the ISP port of the GPRS board.

**Note:** The library will create a new Ubidots device called "**GPRS**". If you desire assign a different device name and label, you can add the add to your code the following lines:

* To change the Device Name:

```
client.setDataSourceName("New_name");
```

* To change the Device label:

```
client.setDataSourceTag("New_label"); 
```

## Send values to Ubidots 

You can send values using the variable label, also you can send values with the context.

### Send values using the variable label

The following example is to send one value to Ubidots, it will create the variable automatically with the label assign by you on the code. If you desire send more than one value just add this line ```client.add("variable_name", value);``` to your code with the parameters needed.

Also, you can find the example on the library examples. Go to **Sketch -> Examples -> Ubidots GPRS library** and select the "**UbidotsSaveValue**" example.

Add your Ubidots TOKEN where indicated, as well as the APN settings.

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
#define VARIABLE_LABEL "Your_variable_label_here" // Assign the variable label 

Ubidots client(TOKEN);
SoftwareSerial gprs = SoftwareSerial(7, 8);
SoftwareSerial *GPRSSerial = &gprs;

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
  float value = analogRead(A0);  // Reading analog pin A0
  client.add(VARIABLE_LABEL, value);
  client.sendAll(); 
  delay(1000);
}
```

### Send value with context

The following example is to send one value with context to Ubidots, it will create the variable automatically with the label assign by you on the code.

Add your Ubidots TOKEN where indicated, as well as the APN settings.

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
#define VARIABLE_LABEL "Your_variable_label_here" // Assign the variable label 

Ubidots client(TOKEN);
SoftwareSerial gprs = SoftwareSerial(7, 8);
SoftwareSerial *GPRSSerial = &gprs;

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
  float value = analogRead(A0);

  // To send geolocation of the variable put it like this:
  client.add(VARIABLE_LABEL, value, "lat=9.92323$lng=1.12323");
  // lng is longitude, and lat is latitude, split it with "$"
  client.sendAll();
  delay(1000);
}
```


## Get value from Ubidots

To get the last value of a variable from Ubidots you can use the following function:

* client.getValueWithDatasource(DEVICE_LABEL, VARIABLE_LABEL);

On the example folder you'll find all the examples codes. Go to **Sketch -> Examples -> Ubidots GPRS library** and select the "**UbidotsGetValue**" example. 

### client.getValueWithDatasource(DEVICE_LABEL, VARIABLE_LABEL)

This function let you get de last value of a variable using the device and variable label.

Add your Ubidots TOKEN where indicated, as well as the APN settings.

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
#define DEVICE_LABEL "Your_device_label_here" // Assign the device label 
#define VARIABLE_LABEL "Your_variable_label_here" // Assign the variable label 

Ubidots client(TOKEN);
SoftwareSerial gprs = SoftwareSerial(7, 8);
SoftwareSerial *GPRSSerial = &gprs;

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
  float value = client.getValueWithDatasource(DEVICE_LABEL, VARIABLE_LABEL);
  Serial.println(value);
  delay(1000);
}
```