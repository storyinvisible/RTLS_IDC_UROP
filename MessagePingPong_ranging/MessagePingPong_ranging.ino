/*
 * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 * Decawave DW1000 library for arduino.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file MessagePingPong.ino
 * Use this to test two-way communication functionality with two
 * DW1000. Both Arduinos use this sketch, but one node configured
 * as initiator/sender of the (first) ping message and the other 
 * being configured as receiver/answerer of the (first) ping message.
 *
 * Configure each node by setting their "trxToggle" attribute to either
 * "SENDER" or "RECEIVER".
 * 
 * @todo
 * - add in SENDER mode timeout if no pong received then resend ping
 */

#include "require_cpp11.h"
#include <SPI.h>
#include <DW1000.h>
#include "DW1000Ranging.h"

// connection pins
constexpr uint8_t PIN_RST = A2; // reset pin
constexpr uint8_t PIN_IRQ = 2; // irq pin
constexpr uint8_t PIN_SS = 10; // spi select pin

// toggle state
enum class TransmissionState : uint8_t {
  SENDER,
  RECEIVER
};
// toggle and message RX/TX
// NOTE: the other Arduino needs to be configured with RECEIVER
//       (or SENDER respectively)
TransmissionState trxToggle = TransmissionState::SENDER;
volatile boolean trxAck = false;
volatile boolean rxError = false;
String msg;
float  distance ;

void setup() {
  // DEBUG monitoring
  Serial.begin(9600);





 //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachBlinkDevice(newBlink);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
  
  //we start the module as an anchor
  DW1000Ranging.startAsAnchor("82:17:5B:D5:A9:9A:E2:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY,0);






  Serial.println(F("### DW1000-arduino-ping-pong-test ###"));
  // initialize the driver
  DW1000.begin(PIN_IRQ, PIN_RST);
  DW1000.select(PIN_SS);
  Serial.println(F("DW1000 initialized ..."));
  // general configuration
  DW1000.newConfiguration();
  DW1000.setDefaults();
  DW1000.setDeviceAddress(1);
  DW1000.setNetworkId(10);
  DW1000.commitConfiguration();
  Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msgInfo[128];
  DW1000.getPrintableDeviceIdentifier(msgInfo);
  Serial.print(F("Device ID: ")); Serial.println(msgInfo);
  DW1000.getPrintableExtendedUniqueIdentifier(msgInfo);
  Serial.print(F("Unique ID: ")); Serial.println(msgInfo);
  DW1000.getPrintableNetworkIdAndShortAddress(msgInfo);
  Serial.print(F("Network ID & Device Address: ")); Serial.println(msgInfo);
  DW1000.getPrintableDeviceMode(msgInfo);
  Serial.print(F("Device mode: ")); Serial.println(msgInfo);
  // attach callback for (successfully) sent and received messages
  DW1000.attachSentHandler(handleSent);
  DW1000.attachReceivedHandler(handleReceived);
  DW1000.attachReceiveFailedHandler(handleReceiveFailed);

  
  // sender starts by sending a PING message, receiver starts listening
  if (trxToggle == TransmissionState::SENDER) {
    msg = "Ping ...";
    receiver();
    transmit();
  } else {
    msg = "... and Pong";
    receiver();
  }
}

void handleSent() {
  // status change on sent success
  trxAck = true;
}

void handleReceived() {
  // status change on received success
  trxAck = true;
}

void handleReceiveFailed() {
  // error flag
  rxError = true;
}

void transmit() {
  DW1000.newTransmit();
  DW1000.setDefaults();
  DW1000.setData(msg);
  DW1000.startTransmit();
}

void receiver() {
  DW1000.newReceive();
  DW1000.setDefaults();
  // so we don't need to restart the receiver manually
  DW1000.receivePermanently(true);
  DW1000.startReceive();
}


void newRange() {
  Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
}

void newBlink(DW1000Device* device) {
  Serial.print("blink; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}




void loop() {
  if (rxError) {
    Serial.println(F("Failed to properly receive message."));
    rxError = false;
    return;
  }
  if (!trxAck) {
    return;
  }
  // continue on any success confirmation
  trxAck = false;
  // a sender will be a receiver and vice versa
  trxToggle = (trxToggle == TransmissionState::SENDER) ? TransmissionState::RECEIVER : TransmissionState::SENDER;
  if (trxToggle == TransmissionState::SENDER) {
    // formerly a receiver
    String rxMsg;
    DW1000.getData(rxMsg);
    Serial.print(F("Received: ")); 
    Serial.println(rxMsg);
    
    DW1000Ranging.loop();
  distance =DW1000Ranging.getDistantDevice()->getRange();
  Serial.print("The distance is \n");
  Serial.print(distance);
  
    transmit();
  } else {
    Serial.print(F("Transmitted: ")); Serial.println(msg);

        DW1000Ranging.loop();
  distance =DW1000Ranging.getDistantDevice()->getRange();
  Serial.print("The distance is \n");
  Serial.print(distance);
  
  }
}
