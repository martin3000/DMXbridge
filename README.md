# DMXbridge

In this project I will use a PC to send Artnet control commands over LAN/Wifi, receive them in a ESP32 and send them out as DMX to a physical device (lixada moving head). It is possible to control the DMX device directly with the ESP32 or you can put a MAX485 in between.

Connections:

Variant a):

GND to DMX 1,
Pin 16 of ESP32 to DMX 2, 
Pint 17 of ESP32 to DMX 3

Variant b):
GND to GND of RS485 module
3.3V to Vcc of RS485 module
