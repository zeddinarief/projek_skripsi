
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

byte msgID = 1;            // count of outgoing messages
byte NodeID = 1;     // address of this device
byte Src = 1;
byte Dst = 0;      // destination to send to
byte nextHop = 0;
String path;

      
void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("|--------------|");
  Serial.println("| LoRa Gateway |");
  Serial.println("|--------------|");
//  Serial.println(sizeof(LoRa));
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded.\nNodeID : ");
  Serial.println(NodeID);
}

void loop() {
 if (Serial.available() > 0) {
  char in = Serial.read();
  if (in == '2'){    
    Dst = 2;   
    search(Dst);
    sendMessage();       
  }
  else if (in == '3'){
    Dst = 3;
    search(Dst) ;
    sendMessage();          
    }
  else if (in == '4'){
    Dst = 4;   
    search(Dst);
    sendMessage();  
    }
    else if (in == '5'){
    Dst = 5;   
    search(Dst);
    sendMessage();   
    }
   }  
    LoRa.receive();
    delay(100);
}

typedef struct
 {
    byte setDst;
    byte setNextHop;
 }  set_tabel[4];

  set_tabel tabel={{2,2},
                   {3,3},
                   {4,2},
                   {5,2}};

void search(byte Dst) {
  for(int x=0; x<4; x++) {  
    if(tabel[x].setDst == Dst){
      nextHop = tabel[x].setNextHop;
    }
  }

}

void sendMessage() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(Src);              // add destination address
  LoRa.write(Dst);             // add sender address
  LoRa.write(nextHop);             // add sender address
  LoRa.write(msgID);                 // add message ID
  LoRa.write(0);
  unsigned long Time = millis(); 
  byte waktu[4];
  waktu[0] = Time;
  waktu[1] = Time >> 8;
  waktu[2] = Time >> 16;
  waktu[3] = Time >> 24;
  LoRa.write(waktu[0]);
  LoRa.write(waktu[1]);
  LoRa.write(waktu[2]);
  LoRa.write(waktu[3]);
  path = String (NodeID);
  LoRa.write(path.length());        // add payload length
  LoRa.print(path);
  LoRa.endPacket();                     // finish packet and send it
  msgID++;                           // increment message ID
  Serial.print("\nSending request to Node : "); 
  Serial.println(Dst);  
  Serial.println("--------------------------");
  
}

void onReceive(int packetSize) {
    if (packetSize == 0) return;          // if there's no packet, return
    byte sender = LoRa.read();          // sender address
    byte recipient = LoRa.read();         // recipient address    
    byte nextNode = LoRa.read();            // Next address
    byte incomingMsgId = LoRa.read();     // incoming msg ID
    byte incomingData = LoRa.read();      // incoming data sensor
    unsigned long lastTime = millis();
    byte delayTime[4];
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
    unsigned long recvTime = (unsigned long)delayTime[0]      |
                             (unsigned long)delayTime[1] << 8 |
                             (unsigned long)delayTime[2] << 16|
                             (unsigned long)delayTime[3] << 24;                             
    double lastDelay = (double)(lastTime - recvTime)/1000;

  if (recipient == NodeID && nextNode == NodeID) {
    Serial.println();
    Serial.println("Response message");
    Serial.println("------------------------");
    Serial.println("Response message from NodeID: " + String(sender, DEC));
    Serial.println("Send to NodeID: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Sensor Data: " + String(incomingData));
    Serial.println("Send and receive time: " + String(lastDelay)+" Sec.");   
    Serial.println("Path traveled :"+String(path));
    Serial.println();
                              // skip rest of function
  }

 else {
//      Serial.println("This message is not for me.");

  }
}
