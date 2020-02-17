#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

byte NodeID = 5;     // address of this device
byte Src = 5;
byte Dst = 0;      // destination to send to
byte NextHop = 0;
byte sensor;
byte delayTime[4];

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial); 
  Serial.println("|-------------|");
  Serial.println("| LoRa Sensor |");
  Serial.println("|-------------|");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, RQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded.\nNode ID : ");
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
 }  Set_tabel[4];

  Set_tabel Tabel={{1,3},
                   {2,4},
                   {3,3},
                   {4,4}};

void search(byte Dst) {
  for(int x=0; x<4; x++) {  
    if(Tabel[x].SetDst == Dst){
      NextHop = Tabel[x].SetNextHop;
    }
  }
  return 0;
}
void sendMessage(byte sensor, byte msgId, byte Src, byte Dst, byte delayTime[]) {
  
  LoRa.beginPacket();            // start packet
  LoRa.write(Src);              // add destination address
  LoRa.write(Dst);             // add sender address
  LoRa.write(NextHop);             
  LoRa.write(msgId);                // add message ID
  LoRa.write(sensor);             // add payload
  LoRa.write(delayTime[0]);
  LoRa.write(delayTime[1]);
  LoRa.write(delayTime[2]);
  LoRa.write(delayTime[3]);
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  byte sender = LoRa.read();          // recipient address
  byte recipient = LoRa.read();            // sender address
  byte nextNode = LoRa.read();            
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();
  delayTime[0] = LoRa.read();
  delayTime[1] = LoRa.read();
  delayTime[2] = LoRa.read();
  delayTime[3] = LoRa.read();
  sensor = random(25,90); 
    
    if (recipient == NodeID && nextNode == NodeID) { // jika penerima paket request adalah node ini
    //    kirim paket balasan
    Serial.println("\nRequest from NodeID : " + String(sender, DEC));
    Serial.println("---------------------");
    Serial.println("\nResponse message");
    Serial.println("Receive from NodeID : " + String(recipient, DEC));
    Serial.println("Send to NodeID : " + String(sender, DEC));
    Serial.println("Next Node : " + String(NextHop, DEC));
    Serial.println("Message ID : "+String(incomingMsgId));
    Serial.println("Sensor data : "+String(sensor));
    Serial.println("");
    search(sender); // method ini mengeset nexthop menuju tujuan
    sendMessage(sensor, incomingMsgId, NodeID, sender, delayTime);
    }       
      
   else {
//    Serial.print("Pesan untuk NodeID: "+String(nextNode));
//    Serial.println("\n This message is not for me");
    }
}



