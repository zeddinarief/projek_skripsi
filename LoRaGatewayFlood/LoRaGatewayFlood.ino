
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgId = 1;            // count of outgoing messages
byte localAddress = 2;     // address of this device
byte destination = 6;      // destination to send to
//byte currentMsgId = 0;
long lastSendTime = 0;        // last send time
int interval = 10000;          // interval between delete old record
int timeout = 3000;
long lastRetransTime = 0;        // last send time

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

  Serial.println("LoRa Gateway Node");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSyncWord(0x34);           // ranges from 0-0xFF, default 0x34, see API docs
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
      Serial.println("mengirim pesan request");
      lastRetransTime = millis();
      sendRequest();
      waitReply();
      Serial.println("");
//      LoRa.receive();
    }
  }
//  parse for a packet, and call onReceive with the result:
//  onReceive(LoRa.parsePacket());
}

void sendRequest() {
//  Serial.println("pesan telah dikirim");
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgId);                 // add message ID
  LoRa.write(0);                       // add message type
  LoRa.write(0);                       // add hopcount
//  payload
  LoRa.write(0);                      // add payload data sensor
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
//  LoRa.write(outgoing.length());        // add payload length
  LoRa.endPacket();                     // finish packet and send it
  msgId++;                           // increment message ID
  Serial.println("pesan telah dikirim");
  Serial.println("menunggu respon");
  
}

void waitReply() {
  int lora = 0;
  int totTimeout = 0;
  while(lora == 0){
    lora = LoRa.parsePacket();
    if (lora > 0) {
      if(!onReceive(lora)){
        lora = 0;
      }      
      Serial.println("ini receive");
      Serial.println("");
    }
    if(totTimeout < 3){
      if (millis() - lastRetransTime > timeout) {
        Serial.println("Retransmisi...");
        sendRequest();
        lastRetransTime = millis();            // timestamp the message
        totTimeout++;
      }
    } else {
      return;
    }
  }
}

bool onReceive(int packetSize) {
  if (packetSize == 0) return 0;          // if there's no packet, return

  // read packet header bytes:
  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte hopcount = LoRa.read();     // incoming msg type
  // read data sensor:
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
  if (recipient == localAddress && incomingMsgType == 1) {
    if(search(sender, recipient, incomingMsgId)){       // jika ada di record
      Serial.println("This message is not for me.");
      Serial.print("id : ");
      Serial.println(incomingMsgId);
      Serial.print("type : ");
      Serial.println(incomingMsgType);
      return 0;
    }
    push(sender, recipient, incomingMsgId);
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message Type: Response");
    Serial.println("Hopcount: " + String(hopcount));
    Serial.print("data sensor Suhu: " + String(incomingData));
    Serial.println(" C");
  //  Serial.println("Message length: " + String(incomingLength));
  //  Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    return 1;
  }
  else {
    Serial.println("This message is not for me.");
    Serial.print("id : ");
    Serial.println(incomingMsgId);
    Serial.print("type : ");
    Serial.println(incomingMsgType);
    return 0;
  }
  
  Serial.println(" ");
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
