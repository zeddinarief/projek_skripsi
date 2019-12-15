
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

//String outgoing;              // outgoing message

byte localAddress = 4;     // address of this device
//byte localAddress = 5;     // address of this device
//byte localAddress = 6;     // address of this device

byte currentMsgType;
long lastSendTime = 0;        // last send time
int interval = 10000;          // interval between delete old record

typedef struct
 {
     byte currentSender;
     byte currentRecipient;
     byte currentMsgId;
 }  record_type;

record_type record[8];

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
  if (millis() - lastSendTime > interval) {
    pop();
//    show();
//    Serial.println("");
    lastSendTime = millis();            // timestamp the message
  }
  if (Serial.available() > 0) {    // is a character available?
    char in = Serial.read();       // get the character
    if (in == 's') {
      show();
      Serial.println();
    }
  }
  //   parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void forwardMessage(int recipient, byte sender, byte msgId, byte msgType, byte sensor) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(recipient);              // add destination address
  LoRa.write(sender);                 // add sender address
  LoRa.write(msgId);                 // add message ID
  LoRa.write(msgType);                 // add message type
//  payload
  LoRa.write(sensor);                   // add data sensor
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte incomingData = LoRa.read();      // incoming data sensor

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress) {
    if(search(sender, recipient, incomingMsgId)){       // jika ada di record
      Serial.print("Message id : ");
      Serial.print(incomingMsgId);
      Serial.println(" has been received before");
      return;
    }
    push(sender, recipient, incomingMsgId);
    forwardMessage(recipient, sender, incomingMsgId, incomingMsgType, incomingData);  
    Serial.print("forward packet id:");
    Serial.println(incomingMsgId);
  } 
  
  Serial.println();
}

void pop() {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].currentSender == 0 && record[x].currentRecipient == 0){
        return;
      }
      record[x].currentSender = record[x+1].currentSender;
      record[x].currentRecipient = record[x+1].currentRecipient;
      record[x].currentMsgId = record[x+1].currentMsgId;
    } else {
      record[x].currentSender = 0;
      record[x].currentRecipient = 0;
      record[x].currentMsgId = 0;
    }
  }
}

void push(byte senderId, byte recipientId, byte packetId) {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].currentSender == 0 && record[x].currentRecipient == 0){
        record[x] = (record_type) {senderId,recipientId,packetId};
        return;
      }
    } else {
      if(record[x].currentSender == 0 && record[x].currentRecipient == 0){
        record[x] = (record_type) {senderId,recipientId,packetId};
      } else {
        pop();
        record[x] = (record_type) {senderId,recipientId,packetId};
      }
    }
  }
}

bool search(byte senderId, byte recipientId, byte packetId) {
  for(int x=0; x<8; x++) {  
    if(record[x].currentSender == senderId && record[x].currentRecipient == recipientId && record[x].currentMsgId == packetId){
      return 1;
    }
  }
  return 0;
}

void show() {
  for(int x=0; x<8; x++) {  
    Serial.print(record[x].currentSender);
    Serial.print("-");
    Serial.print(record[x].currentRecipient);
    Serial.print("-");
    Serial.println(record[x].currentMsgId);
  }
}
