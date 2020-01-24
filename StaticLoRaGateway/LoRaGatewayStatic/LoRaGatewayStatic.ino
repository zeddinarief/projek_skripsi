
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

byte msgID = 1;            // count of outgoing messages
byte nodeID = 1;     // address of this device
byte Src = 1;
byte Dst = 0;      // destination to send to
byte nextHop = 0;

      
void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  
  Serial.println("LoRa Gateway");
//  Serial.println(sizeof(LoRa));
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded.\nNode ID : ");
  Serial.println(nodeID);
}

void loop() {
  char in = Serial.read();
  if (in == '2'){    
    Dst = 2;   
    search(Dst);
    sendMessage();
    Serial.print("\n\nSending request to Node : "); 
    Serial.println(Dst);   
    Serial.print("\n\nSending Time : "); 
    
  }
  else if (in == '3'){
    Dst = 3;
    search(Dst) ;
    sendMessage();
    Serial.print("Sending request to Node : "); 
    Serial.println(Dst);          
    }
  else if (in == '4'){
    Dst = 4;   
    search(Dst);
    sendMessage();
    Serial.print("Sending request to Node : "); 
    Serial.println(Dst);  
    }
    else if (in == '5'){
    Dst = 5;   
    search(Dst);
    sendMessage();
    Serial.print("Sending request to Node : "); 
    Serial.println(Dst);    
    }
  LoRa.receive();
    delay(100);
}

typedef struct
 {
    byte setDst;
    byte setNextHop;
 }  set_tabel[4];

  set_tabel tabel={{2,2},{3,3},{4,2},{5,3}};

void search(byte Dst) {
  for(int x=0; x<4; x++) {  
    if(tabel[x].setDst == Dst){
      nextHop = tabel[x].setNextHop;
    }
  }
  return 0;
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
  LoRa.endPacket();                     // finish packet and send it
  msgID++;                           // increment message ID
  
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

  unsigned long recvTime = (unsigned long)delayTime[0]      |
                           (unsigned long)delayTime[1] << 8 |
                           (unsigned long)delayTime[2] << 16|
                           (unsigned long)delayTime[3] << 24;
                           
  double lastDelay = (double)(lastTime - recvTime)/1000;
                           
  Serial.println();
  Serial.println("Pesan respon");
  Serial.println("------------------------");
  // if the recipient isn't this device or broadcast,
  if (recipient != nodeID) {
    Serial.println("This message is not for me.");
                              // skip rest of function
  }

 else {
    Serial.println("Receive respon from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("data sensor: " + String(incomingData));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println("Last Time : " + String(lastTime));
    Serial.println("Recv Time : " + String(recvTime));
    Serial.println("last Delay: " + String(lastDelay));
    Serial.println();

  }
}
