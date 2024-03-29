/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
#include "DW1000Ranging.h"
//#include "DW1000.h" /////


// connection pins
const uint8_t PIN_RST = A2; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = 10; // spi select pin


byte *shortAddress;

void setup() {
  Serial.begin(9600);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);

  //we start the module as a tag


  //
  //Serial.println(DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  

  DW1000Ranging.startAsTag("FB:F3:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY,0);
    
  
  //added
  shortAddress=DW1000Ranging.getCurrentShortAddress();
  Serial.println(shortAddress[0],HEX);
  Serial.println(shortAddress[1],HEX);
  
  
 }

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}
