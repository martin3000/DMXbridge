# DMXbridge

In this project I will use a PC to send Artnet control commands over LAN/Wifi, receive them in a ESP32 and send them out as DMX to a physical device (lixada moving head). It is possible to control the DMX device directly with the ESP32 or you can put a MAX485 in between.

Connections:

Variant a) without RS485 module:

- GND to DMX 1,
- pin 16 of ESP32 to DMX 2, 
- pin 17 of ESP32 to DMX 3

Variant b) with RS485 module:

- GND to GND of RS485 module
- 3.3V to Vcc of RS485 module
- pin 16 of ESP32 to pin RO of RS485 module
- pin 17 of ESP32 to pin DI of RS485 module
- 3.3V to pin DE of RS485 module
- GND to DMX 1
- A,B of RS485 module to DMX 2 and 3

If the light is reacting, but in the wrong way, then exchange DMX 2 and 3.
 
