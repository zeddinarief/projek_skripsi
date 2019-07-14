
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
byte localAddress = 2;     // address of this device
byte destination = 3;      // destination to send to
//byte destination = 4;      // destination coba
byte currentMsgId = 0;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Gateway Node");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
//  LoRa.onReceive(onReceive);
//  LoRa.receive();
  Serial.print("LoRa init succeeded. Lora node ");
  Serial.println(localAddress);
}

void loop() {
  if (Serial.available() > 0) {    // is a character available?
    char in = Serial.read();       // get the character
    if (in == 's') {
      Serial.println("mengirim pesan request");
      sendRequest();
//      LoRa.receive();
      Serial.println("menunggu respon");
    }
  }
//  parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendRequest() {
//  Serial.println("pesan telah dikirim");
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(0);                       // add message type
  LoRa.write(0);                      // add data sensor
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
//  LoRa.write(outgoing.length());        // add payload length
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  Serial.println("pesan telah dikirim");
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte incomingData = LoRa.read();      // incoming data sensor
//  byte incomingLength = LoRa.read();    // incoming msg length
//  String incoming = "";                 // payload of packet

//  while (LoRa.available()) {            // can't use readString() in callback, so
//    incoming += (char)LoRa.read();      // add bytes one by one
//  }
//
//  if (incomingLength != incoming.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
//    return;                             // skip rest of function
//  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress || incomingMsgType != 1 || incomingMsgId == currentMsgId) {
    Serial.println("This message is not for me.");
//    return;
  }

  // if message is for this device, or broadcast, print details:
//  Serial.println("Received from: 0x" + String(sender, HEX));
//  Serial.println("Sent to: 0x" + String(recipient, HEX));
  else {
    currentMsgId = incomingMsgId;
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message Type: Response");
    Serial.print("data sensor Suhu: " + String(incomingData));
    Serial.println(" C");
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    
  }
  Serial.println(" ");
}
