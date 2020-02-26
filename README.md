# Arduino-GPRS v4.0.0

The GPRS Shield is based on the SIM900 module from SIMCOM. It is compatible with all boards that have the same form factor (and pinout) as a standard Arduino Board. The GPRS Shield is configured and controlled via its UART using simple AT commands. Based on the SIM900 module from SIMCOM, the GPRS Shield is like a cell phone. 

Here you will learn how to interact with the Ubidots API using an Arduino GPRS shield and a Ubidots library.

## Requirements

* [Arduino Uno](http://arduino.cc/en/Main/ArduinoBoardUno)
* [GPRS shield SIM900](http://www.seeedstudio.com/depot/GPRS-Shield-V20-p-1379.html)
* [Arduino IDE 1.6.0 or higher](https://www.arduino.cc/en/Main/Software)
* [Ubidots Arduino GPRS library](https://github.com/ubidots/ubidots-arduino-gprs/archive/master.zip)
* An active SIM card with a data plan 

**NOTE:** This library is not compatible with the old versions. Find the last stable version (**3.0.0**) on the link below:

* [Ubidots Arduino GPRS Libray - 3.0.0.](https://github.com/ubidots/ubidots-arduino-gprs/archive/3.0.0.zip)

Before running the examples, make sure you have an active data plan. You will also need your mobile operator’s APN settings (APN, USER, PASSWORD). You should be able to easily find these settings in Google or in your operator’s website.

## Setup
1. Download the [Arduino IDE](https://www.arduino.cc/en/Main/Software) from the Arduino Website linked here.
2. After it was succesfully downloaded, install it as a normal program on windows, or follow the instructions if you are using linux/Mac. 
3. Open the Arduino IDE, then go to **Sketch -> Include Library -> Manage Libraries**.
4. Finally look for Ubidots GPRS, and install it.

## Alternative Setup
1. Download the [Arduino IDE](https://www.arduino.cc/en/Main/Software) from the Arduino Website linked here.
2. After it was succesfully downloaded, install it as a normal program on windows, or follow the instructions if you are using linux/Mac. 
3. Download the **Ubidots GPRS library** [here](https://github.com/ubidots/ubidots-arduino-gprs/archive/master.zip).
4. Now, click on **Sketch -> Include Library -> Add .ZIP Library**.
5. Select the .ZIP file of Ubidots and then "**Accept**" or "**Choose**".
6. Close the Arduino IDE and open it again.

# Documentation

Before running the examples, make sure you have an active data plan. You will also need your mobile operator’s APN settings (APN, USER, PASSWORD). You should be able to easily find these settings in Google or in your operator’s website.

This library uses the digital PIN "JP" of the SIM900 (PIN 9 in the Arduino) to power on/off the GPRS shield. Make sure the JP pad is soldered. The pad of the JP pin can be found next to the ISP port of the GPRS board.

## Constructor

### Ubidots

```
Ubidots(const char* token, const char* apn, const char* apnUser,const char* apnPass,UbiServer server,IoTProtocol iot_protocol)
```
> @token, [Required]. Your Ubidots unique account [TOKEN](http://help.ubidots.com/user-guides/find-your-token-from-your-ubidots-account).
> @apn, [Required]. Your simcard operator APN setting, usually it's an URL.
> @apnUser, [Required]. Your APN user set by the Simcard operator, some operators don't use an user to authenticate.
> @apnPass, [Required].  Your APN password set by the Simcard operator, some operators don't use a password to authenticate.
> @server, [Optional], [Options] = [`UBI_INDUSTRIAL`, `UBI_EDUCATIONAL`], [Default] = `UBI_INDUSTRIAL`. The server to send data, set `UBI_EDUCATIONAL` if your account is educational type.  
> @iot_protocol, [Optional],[Default] = `UBI_TCP`. The IoT protocol that you will use to send or retrieve data.

Creates an Ubidots instance.

## Methods

```
void add(const char *variable_label, float value, char *context, unsigned long dot_timestamp_seconds, unsigned int dot_timestamp_millis)
```

> @variable_label, [Required]. The label of the variable where the dot will be stored.
> @value, [Required]. The value of the dot.  
> @context, [Optional]. The dot's context.  
> @dot_timestamp_seconds, [Optional]. The dot's timestamp in seconds.  
> @dot_timestamp_millis, [Optional]. The dot's timestamp number of milliseconds. If the timestamp's milliseconds values is not set, the seconds will be multplied by 1000.

Adds a dot with its related value, context and timestamp to be sent to a certain data source, once you use add().

**Important:** The max payload lenght is 700 bytes, if your payload is greater it won't be properly sent. You can see on your serial console the payload to send if you call the `setDebug(bool debug)` method and pass a true value to it.

```
float get(const char* device_label, const char* variable_label)
```

> @device_label, [Required]. The device label which contains the variable to retrieve values from.  
> @variable_label, [Required]. The variable label to retrieve values from.

Returns as float the last value of the dot from the variable.
IotProtocol getCloudProtocol()

```
void addContext(char *key_label, char *key_value)
```

> @key_label, [Required]. The key context label to store values.  
> @key_value, [Required]. The key pair value.

Adds to local memory a new key-value context key. The method inputs must be char pointers. The method allows to store up to 10 key-value pairs.

```
void getContext(char *context)
```

> @context, [Required]. A char pointer where the context will be stored.

Builds the context according to the chosen protocol and stores it in the context char pointer.

```
void setDebug(bool debug)
```

> @debug, [Required]. Boolean type to turn off/on debug messages.

Makes available debug messages through the serial port.

```
bool send(const char* device_label, const char* device_name);
```

> @device_label, [Optional], [Default] = Device's MAC Address. The device label to send data. If not set, the PARTICLE DEVICE ID will be used.  
> @device_name, [Optional], [Default] = @device_label. The device name otherwise assigned if the device doesn't already exist in your Ubidots account. If not set, the device label parameter will be used. **NOTE**: Device name is only supported through TCP/UDP, if you use another protocol, the device name will be the same as device label.  

Sends all the data added using the add() method. Returns true if the data was sent.

```
bool serverConnected();
```
Returns true if the device has a socket opened with Ubidots.

# Examples

Refer to the ![examples](https://github.com/ubidots/ubidots-arduino-gprs/tree/master/examples) folder
