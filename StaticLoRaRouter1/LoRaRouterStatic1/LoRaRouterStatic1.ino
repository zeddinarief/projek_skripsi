#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

byte msgID = 1;            // count of outgoing messages
byte NodeID = 2;                 // address of this device
byte Dst = 0;                // destination to send to
byte NextHop = 0;
byte sensor;
byte delayTime[4];                     // last send time
String path; 

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println(" ---------------");
  Serial.println("| LoRa Router 1 |");
  Serial.println(" ---------------");

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(433E6)) {             
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded. \nNodeID : ");
  Serial.println(NodeID);
}

void loop() {
  delay(100);
  LoRa.receive();
}

  typedef struct
 {
     byte SetDst;
     byte SetNextHop;
 }   Set_tabel[4];

  Set_tabel Tabel={{1,1},
                   {3,3},
                   {4,4},
                   {5,4}};

void search(byte Dst) {
  for(int x=0; x<4; x++) {  
    if(Tabel[x].SetDst == Dst){
      NextHop = Tabel[x].SetNextHop;
    }
  }

}

void sendMessage(byte sensor, byte Src, byte Dst, byte delayTime[]) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(Src);                     // add destination address
  LoRa.write(Dst);                     // add sender address
  LoRa.write(NextHop); 
  LoRa.write(msgId);                 // add message ID
  LoRa.write(sensor);                //data sensor
  LoRa.write(delayTime[0]);
  LoRa.write(delayTime[1]);
  LoRa.write(delayTime[2]);
  LoRa.write(delayTime[3]);
  path = String (NodeID);
  LoRa.write(path.length());        // add payload length
  LoRa.print(path);
  LoRa.endPacket();                     // finish packet and send it
  msgID++;                           // increment message ID
}

void ForwardMessage(byte data, byte msgId, byte Src, byte Dst, byte delayTime[],String path) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(Src);                      // add destination address
  LoRa.write(Dst);                      // add sender address
  LoRa.write(NextHop); 
  LoRa.write(msgId);                 // add message ID
  LoRa.write(data);                      //data sensor
  LoRa.write(delayTime[0]);
  LoRa.write(delayTime[1]);
  LoRa.write(delayTime[2]);
  LoRa.write(delayTime[3]);
  LoRa.write(path.length());        // add payload length
  LoRa.print(path);
  LoRa.endPacket();                     // finish packet and send it

}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return  
  byte sender = LoRa.read();          // sender address
  byte recipient = LoRa.read();         // recipient address    
  byte nextNode = LoRa.read();            // Next address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();      // incoming data sensor
  delayTime[0] = LoRa.read();
  delayTime[1] = LoRa.read();
  delayTime[2] = LoRa.read();
  delayTime[3] = LoRa.read();
  byte pathLength = LoRa.read();    // incoming msg length
  String path = "";                 // payload of packet
  
  while (LoRa.available()) {            // can't use readString() in callback, so
      path += (char)LoRa.read();      // add bytes one by one
    }
  
  if (pathLength != path.length()) {   // check length for error
      return;                             // skip rest of function
    }
    
  path += "-" + String(NodeID);
  sensor = random(25,90);
  Serial.println("\nMessage request!");
  
  if (recipient == NodeID && nextNode == NodeID) { // jika penerima paket request adalah node ini kirim paket balasan
    Serial.println("\nSend message");
    Serial.println("---------------------");
    Serial.println("Receive from NodeID : " + String(recipient, DEC));
    Serial.println("Send to NodeID : " + String(sender, DEC));
    Serial.println("Message ID : "+String(incomingMsgId));
    Serial.println("Sensor data : "+String(sensor));   
    Serial.println("Path traveled :"+String(path));
    Serial.println("");
    search(sender); // method ini mengeset nexthop menuju tujuan
    sendMessage(sensor, NodeID, sender, delayTime, path);
    }       
      
   else if(nextNode == NodeID) {
     Serial.print("\nforward message to NodeID : ");
     Serial.println(recipient);
     search(recipient); // method ini mengeset nexthop menuju tujuan
     ForwardMessage(incomingData,incomingMsgId, sender, recipient,  delayTime, path);       
    }
    
   else {
//    Serial.print(String(nextNode));
//    Serial.println(" This message is not for me");
    }
  
}

