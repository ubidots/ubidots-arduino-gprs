# Arduino-GPRS

The GPRS Shield is compatible with all boards which have the same form factor (and pinout) as a standard Arduino Board. The GPRS Shield is configured and controlled via its UART using simple AT commands. Based on the SIM900 module from SIMCOM, the GPRS Shield is like a cell phone. Besides the communications features, the GPRS Shield has 12 GPIOs, 2 PWMs and an ADC.
Here you will learn how to send a simple or multiple values to the Ubidots API and how to get the last value of one variable of your Ubidots account,using an Arduino GPRS shield and Ubidots library.

## Requirements

* [Arduino uno](http://arduino.cc/en/Main/ArduinoBoardUno)
* [A GPRS shield SIM900](http://www.seeedstudio.com/depot/GPRS-Shield-V20-p-1379.html)
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
This library use a software to power on/off the GPRS shield. For this please solder pad JP. The pad of JP besides the ISP port.
</aside>

## Send one value to Ubidots

To send a value to Ubidots, go to **Sketch -> Examples -> Ubidots Arduino GPRS library** and select the "saveValue" example. 
Put your Ubidots token and variable ID where indicated,  as well as the APN settings
Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

```c++
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
```


## Get one value from Ubidots

To get the last value of a variable from Ubidots, go to **Sketch -> Examples -> Ubidots Arduino GPRS library** and select the "getValue" example. 
Put your Ubidots token and variable ID where indicated,  as well as the APN settings
Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

```c++
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
	  float value = client.getValue(ID);
	  Serial.println(value);
	  delay(1000);
	  Serial.println(value);
	}

```

## Send multiple values to Ubidots 

To send multiple values to Ubidots, go to **Sketch -> Examples -> Ubidots Arduino GPRS library** and select the "saveValues" example. 
Put your Ubidots token and variable ID's where indicated,  as well as the APN settings
Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

```c++
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
	  // client.add(ID1, value_1, "Lat:9.786589", "Long:1.8688797");

	  client.add(ID1, value_1, "Lat:9.786589", "Long:1.8688797");
	  client.add(ID2,value_2, "", "");
	  client.add(ID3,value_3, "", "");

	  client.sendAll();
	}
```
