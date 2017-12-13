# Cayenne MQTT C Library
The Cayenne MQTT C Library provides functions to easily connect to the Cayenne IoT project builder.

This library bundles the [Eclipse Paho MQTT C/C++ client](https://github.com/eclipse/paho.mqtt.embedded-c).

## Repository Structure
- **src** - The library source code.
  - **CayenneMQTTClient** - Platform independent Cayenne C library using the Paho MQTT C library. To create platform specific versions of this library networking and timer code for the platform are required.
  - **CayenneUtils** - Common code for creating and parsing Cayenne topics and payloads. This code can be used with any MQTT client.
  - **MQTTCommon** - Common Paho MQTT C code used by both the C and C++ libraries.
  - **Platform** - Platform specific networking and timer code, as well as test and example applications.
    - **Linux** - Linux C networking and timer code, as well as test and example applications. Test and example files can be built using the makefile.
    
## Adding Additional Platforms
The Cayenne MQTT client code is platform independent but it requires platform specific code to create timers and to read and write data over the network. To add support for additional platforms you will need to create platform specific timer and networking code.
  - **Timer** - This struct and associated functions are used to create countdown timers. They are described in the [PlatformHeader.h](https://github.com/myDevicesIoT/Cayenne-MQTT-C/blob/master/src/CayenneMQTTClient/PlatformHeader.h) file. An example implementation for Linux is under the [Platform/Linux](https://github.com/myDevicesIoT/Cayenne-MQTT-C/tree/master/src/Platform/Linux) folder.
  - **Network** - This struct and associated functions are used to read and write data over the network. They are described in the [PlatformHeader.h](https://github.com/myDevicesIoT/Cayenne-MQTT-C/blob/master/src/CayenneMQTTClient/PlatformHeader.h) file. An example implementation for Linux is under the [Platform/Linux](https://github.com/myDevicesIoT/Cayenne-MQTT-C/tree/master/src/Platform/Linux) folder.
  
After creating new platform specific timer and networking code you can include their headers in [PlatformHeader.h](https://github.com/myDevicesIoT/Cayenne-MQTT-C/blob/master/src/CayenneMQTTClient/PlatformHeader.h) to make the Cayenne MQTT client use your code.
    
## Cayenne MQTT Libraries
- **[Cayenne-MQTT-C](https://github.com/myDevicesIoT/Cayenne-MQTT-C)** - C version of the Cayenne MQTT Library.
- **[Cayenne-MQTT-CPP](https://github.com/myDevicesIoT/Cayenne-MQTT-CPP)** - C++ version of the Cayenne MQTT Library.
- **[Cayenne-MQTT-Arduino](https://github.com/myDevicesIoT/Cayenne-MQTT-Arduino)** - Arduino version of the Cayenne MQTT Library.
- **[Cayenne-MQTT-mbed](https://github.com/myDevicesIoT/Cayenne-MQTT-mbed)** - mbed version of the Cayenne MQTT Library. This is also available on the mbed developer site [here](https://developer.mbed.org/teams/myDevicesIoT/code/Cayenne-MQTT-mbed/).
