# Timer Relay

This is Wemos D1 mini code for a 2-Relay Timer switch.
The Wemos is used to read the time from a DS 3231 RTC module via I2C, and when a configured "Start Time" is reached, a relay is turned ON, via a GPIO pin. When the time reaches a configured "End Time", the relay is turned OFF. 
There are two such relays which can be configured independently.

The Wemos runs a WiFi hotspot and gets the IP address 192.168.4.1. It runs a web-server serving a configuration page which can be accessed at http://192.168.4.1/  from a mobile phone or laptop computer connected to the Wemos' hotspot. This configuration page allows the user to set the RTC time, and the two timers. It also shows the times when the timers become ON and OFF the next time.
The config page looks like the following:

<img src="https://github.com/ajithvasudevan/TimerRelay/raw/master/TimerRelay%20-%20Web%20UI.png" alt="Drawing" width="400px"/>

The circuit can be powered either directly by a 5V DC power source as shown in the schematic diagram below, or a USB (mobile phone) charger connected to the USB port on the Wemos.


## Schematic

![Schematic](https://github.com/ajithvasudevan/TimerRelay/raw/master/TimerRelay.png)
