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
byte destination = 6;      // destination to send to
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
  char in = Serial.read();
  if (in == 's'){    
    sendMessage();
    Serial.println("Sending request to sensor  ");    
  }
  LoRa.receive();
    delay(100);
}

void sendMessage() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(0);
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}



void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();      // incoming data sensor
 
  Serial.print("Dikirim ke : ");
  Serial.println(destination);
  Serial.println("------------------------\n");
  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress) {
    Serial.println("This message is not for me.");
                              // skip rest of function
  }

 else {
    Serial.println("\nReceive respon from: " + String(sender, DEC));
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
