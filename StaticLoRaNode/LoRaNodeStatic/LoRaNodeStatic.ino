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
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial); 

  Serial.println("LoRa Sensor");

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
    sensor = random(100);  
    delay(100); 
    LoRa.receive();
}

 typedef struct
 {
    byte SetDst;
    byte SetNextHop;
 }  Set_tabel[4];

  Set_tabel Tabel={{1,3},{2,4},{3,3},{4,4}};

void search(byte Dst) {
  for(int x=0; x<4; x++) {  
    if(Tabel[x].SetDst == Dst){
      NextHop = Tabel[x].SetNextHop;
    }
  }
  return 0;
}
void sendMessage(byte msgId, byte Src, byte Dst) {
  
  LoRa.beginPacket();            // start packet
  LoRa.write(Src);              // add destination address
  LoRa.write(Dst);             // add sender address
  LoRa.write(NextHop);             
  LoRa.write(msgId);                // add message ID
  LoRa.write(sensor);             // add payload
  LoRa.endPacket();                     // finish packet and send it
//  msgCount++;                           // increment message ID
//  Serial.println("isi paket > "+outgoing);
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  byte sender = LoRa.read();          // recipient address
  byte recipient = LoRa.read();            // sender address
  byte NextNode = LoRa.read();            
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();

    Serial.println("\nRequest diterima");
    Serial.println("Received from: " + String(sender, DEC));
    Serial.println("For NodeID: " + String(recipient, DEC));
    Serial.println("Message Id : "+ String(incomingMsgId) );
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println("---------------------");
    
    if (recipient == NodeID) { // jika penerima paket request adalah node ini
    //    kirim paket balasan
    search(sender); // method ini mengeset nexthop menuju tujuan
    sendMessage(incomingMsgId, NodeID, sender);
    Serial.println("\nMengirim Pesan");
    Serial.println("Send to : " + String(sender, DEC));
    Serial.println("From id: " + String(recipient, DEC));
    Serial.println("Data Sensor : "+String(sensor));
    Serial.println("Message Id : "+String(incomingMsgId));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    }       
      
   else {
    Serial.print(String(NextNode));
    Serial.println(" This message is not for me");
    }
}



