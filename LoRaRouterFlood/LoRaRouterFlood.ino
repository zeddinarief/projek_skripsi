
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages

//byte localAddress = 4;     // address of this device
//byte localAddress = 5;     // address of this device
byte localAddress = 6;     // address of this device

//byte destination = 5;      // destination coba
byte currentMsgId = 0;
byte currentMsgType;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Relay Node");

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
  //   parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void forwardMessage(int recipient, byte sender, byte msgId, byte msgType, byte sensor) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(recipient);              // add destination address
  LoRa.write(sender);             // add sender address
  LoRa.write(msgId);                 // add message ID
  LoRa.write(msgType);                 // add message type
  LoRa.write(sensor);                   // add data sensor
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
//  LoRa.write(outgoing.length());        // add payload length
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte incomingData = LoRa.read();      // incoming data sensor

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress) {
//    Serial.println("This message is not for me.");
    if (currentMsgId != incomingMsgId){
      currentMsgId = incomingMsgId;
      currentMsgType = incomingMsgType;
      forwardMessage(recipient, sender, incomingMsgId, incomingMsgType, incomingData);  
      Serial.print("forward paket id : ");
      Serial.println(incomingMsgId);
//      LoRa.receive();                     // go back into receive mode
    } else {
      if (incomingMsgType != currentMsgType){
        forwardMessage(recipient, sender, incomingMsgId, incomingMsgType, incomingData);
        Serial.print("forward paket id : ");
        Serial.println(incomingMsgId);
      } else {
        Serial.print("Message id : ");
        Serial.print(incomingMsgId);
        Serial.println(" has been received before");  
      }
//      return;                             // skip rest of function
    }
  } else {
//    return;
    Serial.print("Message id : ");
    Serial.print(incomingMsgId);
    Serial.println(" is not for this node");
    // if message is for this device, or broadcast, print details:
  //  Serial.println("Received from: 0x" + String(sender, HEX));
  //  Serial.println("Sent to: 0x" + String(recipient, HEX));
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("data sensor: " + String(incomingData));
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));  
  }
  Serial.println();
}

