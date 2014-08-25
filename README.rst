Ubidots-Arduino
===============

Ubidots-Arduino is an Arduino library for interacting with Ubidots through its API. 
The library also contains the code to connect to your WiFi network.

Note: This code has been tested ONLY with the official Arduino WiFi Shield: http://arduino.cc/en/Main/ArduinoWiFiShield
We can't guarantee that it will work with different WiFi modules.

Install the Library
-------------------

1. Download the library as a ZIP file here: https://github.com/ubidots/ubidots-arduino/blob/master/Library/Ubidots.zip?raw=true

2. Open the Arduino IDE and go to "Sketch" --> "Import Library" --> "Add Library".

3. Select the downloaded ZIP file

4. Restart the Arduino IDE, you should then see Ubidots examples under "File" --> "Examples" --> "Ubidots"


A Quick example
----------------
Here's a quick example of how to use the library, with the serial terminal of Arduino:


.. code-block:: cpp

  /*
  Analog Read example for Ubidots 

  This sketch connects to Ubidots (http://www.ubidots.com) using an Arduino Wifi shield.

  This example is written for a network using WPA encryption. You only need your SSID and PASS.

  This example uses Ubidots official library for Arduino WiFi Shield, for the Ubidots API v1.6.

  With this example you can read an analog sensor with the arduino, and post it
  to the Ubidots platform with the function save_value 

  Components:
  * Arduino Wifi shield
  * Serial Monitor in your Arduino IDE

  Created 9 Jun 2014
  Modified 9 Jun 2014
  by Mateo Vélez

  This code is in the public domain.

  */
  
  #include <WiFi.h>
  #include <Ubidots.h>
  char ssid[] = "Atom House Medellin";                                                 //your network SSID (name) 
  char pass[] = "atommed2014";                                                         //your network password (use for WPA, or use as key for WEP)
  String api = "5ca9b10038e49e0492c6794f9043f0918ddcbd26";                             //your API Key number
  String idvari = "53baaf3c76254244e1c8e408";                                          //the number of the Ubidots variable
  int sensorValue = 0;                                                                 //variable to store the value coming from the sensor
  int sensorPin = A0;                                                                  //select your analog pin
  Ubidots ubiclient(api);                                                              //call the api with the function ubiclient
  
   
  void setup()
  {
    boolean response;                                                                 
    int status = WL_IDLE_STATUS;                                                      //we need to define first a WL_IDLE_STATUS for the network
    Serial.begin(9600);                                                               //9600 baud for serial transmision
    response = ubiclient.WifiCon(ssid, pass, status, api);                               //this function is to connect to your wifi network
    Serial.println(response);                                                         //print response to the Serial Monitor
  }
  
  void loop()
  {
    sensorValue = analogRead(sensorPin); 
    if (ubiclient.save_value(idvari, String(sensorValue)))                         //this function is to post to ubidots and return True or False depending on the connection status
    {
      Serial.println("The sensor value " + String(sensorValue) + " was sent to Ubidots");                //print the sensor value     
    }     
  }


API Reference
-------------

Ubidots ubiclient():
````````````````````
.. code-block:: cpp

    Ubidots ubiclient(api);

=======  ========  =================================
Type     Argument  Description
=======  ========  =================================
String   api       Your API key for the Ubidots API
=======  ========  =================================

Initialize a Ubidots client. This is most likely to be the first Ubidots library function to call.

save_value()
````````````````````
.. code-block:: cpp

    boolean = ubiclient.save_value(idvari, String(incomingByte), ctext)
=======  ============  ===================================
Type     Argument      Description
=======  ============  ===================================
String   idvari        ID of the variable to save
String   incomingByte  The value of the sensor
String   ctext         Content text of the value (optional)
=======  ============  ====================================

Save a value to Ubidots. Returns true upon success. Returns false upon error.

Please pay close attention to the format of ctext:

.. code-block:: cpp

   String ctext = "{\"attribute\":\"attribute_value\",\"attribute\":\"attribute_value_2\"}";

Example:

.. code-block:: cpp

  String ctext = "{\"color\":\"blue\",\"status\":\"active\"}";
   
Sending the context is only an option; it's not mandatory.

get_value()
```````````
.. code-block:: cpp

    readvalue = ubiclient.get_value(idvari);

==================  ===========  =============================================
Type                Argument     Description
==================  ===========  =============================================
String              idvari       ID of the variable that you want make request 
==================  ===========  =============================================

Get value from Ubidots. Returns a String containing the last value of the variable.

WifiCon()
`````````
.. code-block:: c

    response = ubiclient.WifiCon(ssid, pass, status, api);

==============  ===========  =================================================
Type            Argument     Description
==============  ===========  =================================================
String          ssid         The SSID of your WiFi network
String          pass         The pass of your WiFi network
String          status       is the value of the initialization status of WiFi
String          api          Your API Key number
==============  ===========  =================================================

This Function is to connect to your WiFi network, after connection it creates a token using the API key. 
Returns a boolean (true or false) depending on whether the token is obtained or not. 
