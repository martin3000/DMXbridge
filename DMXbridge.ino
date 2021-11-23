/*
 * (c) 2021 by JMS 
 * transmit received ArtNet messages (DMX) out to the rs485 shield to control a physical DMX device.
 * see https://github.com/someweisguy/esp_dmx#configuring-the-dmx-port
 * see https://github.com/someweisguy/esp_dmx/issues/7
 * Libraries:
 * - esp_dmx 1.1.2 from Mitch Weisbrod
 * - ArtnetWifi 1.4.0 from Stephan Ruloff 
 * Board Manager:
 * - esp32 2.0.0 from espressif
 */

#include <WiFi.h>
#include <Arduino.h>
#include <ArtnetWifi.h>
#include <esp_dmx.h>

/* First, lets define the hardware pins that we are using with our ESP32. We
  need to define which pin is transmitting data and which pin is receiving data.
  DMX circuits also often need to be told when we are transmitting and when we are
  receiving data. We can do this by defining an enable pin. */
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;

/* DMX controller to use. Controller 0 is typically used to transmit serial data back to your Serial Monitor,
  so we shouldn't use that controller. Lets use controller 2! */
dmx_port_t dmxPort = UART_NUM_2;

/* Now we want somewhere to store our DMX data. Since a single packet of DMX
  data can be up to 513 bytes long, we want our array to be at least that long.
  This library knows that the max DMX packet size is 513, so we can fill in the
  array size with `DMX_MAX_PACKET_SIZE`. */
  byte buf[DMX_MAX_PACKET_SIZE];


//Wifi settings
const char* ssid = "SSID123456";
const char* password = "secret";

WiFiUDP UdpSend;
ArtnetWifi artnet;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void) {
  boolean state = true;
  int i = 0;

  WiFi.hostname("DMXbridge");
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(IPAddress(WiFi.localIP()));
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  uint16_t l;
  
  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Seq: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data (");
  Serial.print(length, DEC);
  Serial.print("): ");

  if (length > 16) {
    l = 16;
  } else {
    l = length;
  }
  // send out the buffer
  for (int i = 0; i < l; i++)  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  if (length>16) {
    Serial.print("...");
  }
  Serial.println();

  /* write data to dmx. ignore the universe number */
  if (length > DMX_MAX_PACKET_SIZE-1) {
    Serial.println("WARNING: DMX packet too big, truncated!");
    length = DMX_MAX_PACKET_SIZE-1;
  }

  memcpy(buf+1, data, length);    //fill slot 1-512
  buf[0]=DMX_SC;                  //slot 0
  
  dmx_write_packet(dmxPort, buf, length+1);
}


void setup(){
  // set-up serial for debug output
  Serial.begin(115200);
  ConnectWifi();

  /* Configure the DMX hardware to the default DMX settings and tell the DMX
      driver which hardware pins we are using. */
  dmx_config_t dmxConfig = DMX_DEFAULT_CONFIG;
  dmx_param_config(dmxPort, &dmxConfig);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  /* install the DMX driver. We'll tell it which DMX port to use and
    how big our DMX packet is expected to be. Typically, we'd pass it a handle
    to a queue, but since we are only transmitting DMX, we don't need a queue.
    We can write `NULL` where we'd normally put our queue handle. We'll also
    pass some interrupt priority information. The interrupt priority can be set
    to 1. */
  int queueSize = 0;
  int interruptPriority = 1;
  dmx_driver_install(dmxPort, DMX_MAX_PACKET_SIZE, queueSize, NULL, interruptPriority);

  /* Finally, since we are transmitting DMX, we should tell the DMX driver that
    we are transmitting, not receiving. We should also set our DMX start code
    to 0.*/
  dmx_set_mode(dmxPort, DMX_MODE_TX);

  /* initialize the transmit buffer */
  for(int i = 0; i<DMX_MAX_PACKET_SIZE; i++){
    buf[i] = 0;
  }
  dmx_write_packet(dmxPort, buf, DMX_MAX_PACKET_SIZE);

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void loop() {
  // we call the read function inside the loop
  artnet.read();

  /* transmit the DMX packet */
  /* DMX devices need a continuous stream auf data so we continuously send the last packet */ 
  dmx_tx_packet(dmxPort);

  delay(100);
  
  /* If we have no more work to do, we will wait until we are done sending our DMX packet. */
  dmx_wait_tx_done(dmxPort, DMX_TX_PACKET_TOUT_TICK);
}
