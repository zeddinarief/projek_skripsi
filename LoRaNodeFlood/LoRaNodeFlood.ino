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
#include <DHT.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
DHT dht(3, DHT11);

String outgoing;              // outgoing message

byte msgId = 1;            // count of outgoing messages

byte localAddress = 6;     // address of this device
byte destination = 2;      // destination to send to
byte suhu;
long lastPop = 0;        // last pop time
int interval = 10000;          // interval between delete old record

typedef struct              // untuk menyimpan record paket yang pernah lewat
 {
     byte currentSender;
     byte currentMsgId;
 }  record_type;

record_type record[8];

void setup() {
  Serial.begin(19200);                   // initialize serial
  dht.begin();
  while (!Serial);

  Serial.println("LoRa Node Sensor");

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
  
  suhu = dht.readTemperature();
//  suhu = 20;
  onReceive(LoRa.parsePacket());
}

void sendMessage(byte dest, byte id, byte sendTime[], byte sensor, byte reqId) {
//void sendMessage(byte dest, byte id, byte sensor, byte reqId) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(dest);              // add destination address
  LoRa.write(localAddress);       // add sender address
  LoRa.write(id);                 // add message ID
  LoRa.write(1);                 // add message type
  LoRa.write(reqId);             // add id pkt request
  LoRa.write(sensor);             // add payload data sensor
  LoRa.write(sendTime[0]);          // add sendTime
  LoRa.write(sendTime[1]);          // add sendTime
  LoRa.write(sendTime[2]);          // add sendTime
  LoRa.write(sendTime[3]);          // add sendTime
  //  path info
  String path = String(localAddress);
  LoRa.write(path.length());        // add path length
  LoRa.print(path);                 // add path
  LoRa.endPacket();                     // finish packet and send it
  msgId++;
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
//  Serial.println("paket size : " + String(packetSize));
  
  // read packet header bytes:
  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
//  send time info
  byte waktu[4];
  waktu[0] = LoRa.read();
  waktu[1] = LoRa.read();
  waktu[2] = LoRa.read();
  waktu[3] = LoRa.read();
  byte pathLength = LoRa.read();    // incoming msg length
  String path = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    path += (char)LoRa.read();      // add bytes one by one
  }

  if (pathLength != path.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }
  path += "-" + String(localAddress);

  Serial.println("Receiving request ....");

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress || incomingMsgType != 0) {
    Serial.print("This message id : ");
    Serial.println(incomingMsgId);
    Serial.print("from : ");
    Serial.print(sender);
    Serial.println(" is not for me.");
    Serial.println("");
    return;                             // skip rest of function
  }

  if(search(sender, incomingMsgId)){       // jika ada di record
    Serial.print("This message id : ");
    Serial.print(incomingMsgId);
    Serial.println(" is not for me.");
    Serial.println("");
    return;
  }
    
//  byte newhopcount = hopcount + 1;
  Serial.print("suhu: ");
  Serial.print(suhu);
  Serial.println(" C");
  
  delay(20);
  sendMessage(sender, msgId, waktu, suhu, incomingMsgId);
//  sendMessage(sender, msgId, suhu, incomingMsgId);
  Serial.println("Sending response ...");
//  LoRa.receive();
  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: " + String(sender, DEC));
  Serial.println("Sent to: " + String(recipient, DEC));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message type: Request");
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println(path);
  Serial.println();
  push(sender, incomingMsgId);
}

void pop() {
  for(int x=0; x<8; x++) {  
    if(x < 7){      // index bukan yg terakhir
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
