# Timer Relay

This is Wemos D1 mini code for a 2-Relay Timer switch.
The Wemos is used to read the time from a DS 3231 RTC module via I2C, and when a configured "Start Time" is reached, a relay is turned ON, via a GPIO pin. When the time reaches a configured "End Time", the relay is turned OFF. 
There are two such relays which can be configured independently.

The Wemos runs a WiFi hotspot and gets the IP address 192.168.4.1. It runs a web-server which can be accessed at http://192.168.4.1/  from a mobile phone or laptop computer connectd to the Wemos' hotspot.

The page at this URL allows the user to set the RTC time, and the two timers.

## Schematic

![Schematic](https://github.com/ajithvasudevan/TimerRelay/raw/master/TimerRelay.png)
