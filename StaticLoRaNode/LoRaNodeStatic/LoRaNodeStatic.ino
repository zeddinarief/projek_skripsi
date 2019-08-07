/*
  LoRa Duplex communication

  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback. For an

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgCount = 1;            // count of outgoing messages

//byte localAddress = 0xBB;     // address of this device
//byte destination = 0xFF;      // destination to send to
byte localAddress = 6;     // address of this device
byte destination = 5;      // destination to send to
byte sensor;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends



void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial); 

  Serial.println("LoRa Sensor");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, RQ pin

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
    delay(100);
LoRa.receive();
}

void sendMessage(byte sensor, byte msgId) {
  
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgId);                // add message ID
  LoRa.write(sensor);
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
//  msgCount++;                           // increment message ID
//  Serial.println("isi paket > "+outgoing);
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();
//  byte incomingLength = LoRa.read();    // incoming msg length
//  String incoming = "";

//  while (LoRa.available()) {
//    incoming += (char)LoRa.read();
//  }
//
//  if (incomingLength != incoming.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
//    return;                             // skip rest of function
//  }

  // if the recipient isn't this device or broadcast,
    if (recipient != localAddress) {
      Serial.println(recipient);
      
      Serial.println("This message is not for me.");
      return;                             // skip rest of function
    }
   else{
    // if message is for this device, or broadcast, print details:
    Serial.println("\nPesan diterima");
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("For id: " + String(recipient, DEC));
    Serial.print("Data Sensor : ");
    Serial.println(String(incomingData));
  //  Serial.println("Message ID: " + String(incomingMsgId));
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
    delay(200);
    sendMessage(incomingData, incomingMsgId);  
    
  }
}

