
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
long lastPop = 0;        // last pop time
int interval = 10000;          // interval between delete old record
int timeout = 3000;
long lastRetransTime = 0;        // last send time

typedef struct                 // untuk menyimpan record paket yang pernah lewat
 {
     byte currentSender;
     byte currentMsgId;
 }  record_type;

record_type record[8];

void setup() {
  Serial.begin(19200);                   // initialize serial
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
  if (millis() - lastPop > interval) {
    pop();
//    show();
//    Serial.println("");
    lastPop = millis();            // timestamp the message
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
  onReceive(LoRa.parsePacket());
}

void sendRequest() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgId);                 // add message ID
  LoRa.write(0);                       // add message type
//  add send time info
  unsigned long timeSend = millis();
//  Serial.print("time : ");
//  Serial.println(timeSend);
  byte waktu[4];              // convert time to 4 byte array
  waktu[0] = timeSend;
  waktu[1] = timeSend >> 8;
  waktu[2] = timeSend >> 16;
  waktu[3] = timeSend >> 24;
  LoRa.write(waktu[0]);                       // add sendTime
  LoRa.write(waktu[1]);                       // add sendTime
  LoRa.write(waktu[2]);                       // add sendTime
  LoRa.write(waktu[3]);                       // add sendTime
//  path info
  String path = String(localAddress);
  LoRa.write(path.length());        // add path length
  LoRa.print(path);                 // add path
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
  if (packetSize < 10) return 0;          // if there's no packet match, return 0 or false
//  Serial.println("paket size : " + String(packetSize));
  unsigned long recvTime = millis();          // receive packet time
  // read packet header bytes:
  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte reqId = LoRa.read();           // id paket request
  // read data sensor
  byte incomingData = LoRa.read();      // incoming data sensor
  //  packet time send
  byte waktu[4];
  waktu[0] = LoRa.read();
  waktu[1] = LoRa.read();
  waktu[2] = LoRa.read();
  waktu[3] = LoRa.read();
//  path info
  byte pathLength = LoRa.read();    // incoming msg length
  String path = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    path += (char)LoRa.read();      // add bytes one by one
  }
  
  if (pathLength != path.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
    return 0;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient == localAddress && incomingMsgType == 1) {
    if(search(sender, incomingMsgId)){       // jika ada di record
      Serial.println("This message is not for me.");
      Serial.print("id : ");
      Serial.println(incomingMsgId);
      Serial.print("type : ");
      Serial.println(incomingMsgType);
      return 0;                // if there's no packet match, return 0 or false
    }
    push(sender, incomingMsgId);
    unsigned long sendTime = (unsigned long)waktu[3] << 24 
                          | (unsigned long)waktu[2] << 16
                          | (unsigned long)waktu[1] << 8
                          | (unsigned long)waktu[0];
    
    double rtt = (double)(recvTime - sendTime) / 2000;
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Request ID: " + String(reqId));
    Serial.println("Message Type: Response");
    Serial.print("Round trip time(RTT): ");
    Serial.println(rtt, 3);
    Serial.print("Data Suhu: " + String(incomingData));
    Serial.println(" C");
  //  Serial.println("Message length: " + String(incomingLength));
    Serial.println("Path: " + path + "-" + String(localAddress));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    return 1;                  // if there's packet match, return 1 or true
  }
  else {
    Serial.println("This message is not for me.");
    Serial.print("id : ");
    Serial.println(incomingMsgId);
    Serial.print("type : ");
    Serial.println(incomingMsgType);
    return 0;                 // if there's no packet match, return 0 or false
  }
  
  Serial.println(" ");
}

void pop() {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].currentSender == 0){
        return;
      }
      record[x].currentSender = record[x+1].currentSender;
      record[x].currentMsgId = record[x+1].currentMsgId;
    } else {
      record[x].currentSender = 0;
      record[x].currentMsgId = 0;
    }
  }
}

void push(byte senderId, byte packetId) {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].currentSender == 0){
        record[x] = (record_type) {senderId,packetId};
        return;
      }
    } else {
      if(record[x].currentSender == 0){
        record[x] = (record_type) {senderId,packetId};
      } else {
        pop();
        record[x] = (record_type) {senderId,packetId};
      }
    }
  }
}

bool search(byte senderId, byte packetId) {
  for(int x=0; x<8; x++) {  
    if(record[x].currentSender == senderId && record[x].currentMsgId == packetId){
      return 1;
    }
  }
  return 0;
}

void show() {
  for(int x=0; x<8; x++) {  
    Serial.print(record[x].currentSender);
    Serial.print("-");
    Serial.println(record[x].currentMsgId);
  }
}
