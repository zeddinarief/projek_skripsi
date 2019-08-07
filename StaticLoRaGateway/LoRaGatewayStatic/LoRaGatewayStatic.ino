/*
  LoRa Duplex communication wth callback

  Sends a message every half second, and uses callback
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Note: while sending, LoRa radio is not listening for incoming messages.
  Note2: when using the callback method, you can't use any of the Stream
  functions that rely on the timeout, such as readString, parseInt(), etc.

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgCount = 1;            // count of outgoing messages
byte localAddress = 3;     // address of this device
byte destination = 5;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  
  Serial.println("LoRa Gateway");
  Serial.println(sizeof(LoRa));
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded.\nId : ");
  Serial.println(localAddress);
}


void loop() {
//  if (millis() - lastSendTime > interval) {
//    String message = "HeLoRa World!";   // send a message
////    sendMessage(message);
////    Serial.println("Sending " + message);
//    lastSendTime = millis();            // timestamp the message
//    interval = random(2000) + 1000;     // 2-3 seconds
////    LoRa.receive();                     // go back into receive mode
//  
//
//  Serial.println("replay"); 
//  LoRa.onReceive(onReceive);
//  LoRa.receive();
//  }

  char in = Serial.read();
//    String message = "HeLoRa World! akhirnya bisa hiya hiya hiya..... :) ";   // send a message
    byte data = 100;
        
  if (in == 's'){    
    sendMessage(data);
    Serial.println("Sending  " + String(data));    
  }
  LoRa.receive();
    delay(100);
}

void sendMessage(byte sensor) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sensor);
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void Replay(int balesan){
  LoRa.beginPacket();                   // start packet
  LoRa.write(balesan);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.endPacket();
  Serial.println("replay....pesan diterima\n"); 
  LoRa.receive();
  }

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();      // incoming data sensor
//  byte incomingMsg = LoRa.read();
//  byte incomingLength = LoRa.read();    // incoming msg length
//  String incoming = "";                 // payload of packet

 

//  while (LoRa.available()) {            // can't use readString() in callback, so
//    incoming += (char)LoRa.read();      // add bytes one by one
//  }

//  if (incomingLength != incoming.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
//    return;                             // skip rest of function
//  }

  Serial.print("Dikirim ke : ");
  Serial.println(recipient);
  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress) {
    Serial.println("This message is not for me.");
   
//    LoRa.beginPacket();                   // start packet
//    LoRa.write(destination);              // add destination address
//    LoRa.write(localAddress);             // add sender address
//    LoRa.write(incomingMsgId);                 // add message ID
//    LoRa.write(incomingData);                 // add data sensor
////    LoRa.write(outgoing.length());        // add payload length
////    LoRa.print(outgoing);                 // add payload
//    LoRa.endPacket();                     // finish packet and send it
//    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
//  Serial.println("Received from: 0x" + String(sender, HEX));
//  Serial.println("Sent to: 0x" + String(recipient, HEX));
  else {
    Serial.println("Receive respon from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("data sensor: " + String(incomingData));
//    Serial.println("isi Paket: " + String(incoming));
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    
//    Serial.println("Besar paket: " + String(paketSize);
    Serial.println();

//    sendMessage("");
//    Replay(destination);
    
  }
}
