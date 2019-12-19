#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgCount = 1;            // count of outgoing messages
byte NodeID = 3;                 // address of this device
byte Src = 0;
byte Dst = 0;                // destination to send to
byte NextHop = 0;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("LoRa Router");

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded. Lora node : ");
  Serial.println(localAddress);
}

void loop() {
 }
  delay(100);
  LoRa.receive();
}

void sendMessage(byte sensor, byte msgId, byte Src, byte Dst, byte NextHop) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(Src);              // add destination address
  LoRa.write(Dst);             // add sender address
  LoRa.write(NextHop); 
  LoRa.write(msgId);                 // add message ID
  LoRa.write(sensor);
  LoRa.endPacket();                     // finish packet and send it
  Serial.print("menuju :");
  Serial.println(String(Dst, DEC));
  
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  
  byte sender = LoRa.read();          // sender address
  byte recipient = LoRa.read();         // recipient address    
  byte NextNode = LoRa.read();            // Next address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();      // incoming data sensor

  if (NextNode == NodeID) {
    if (sender == 1 && recipient == 5){
          Dst = 5;
          NextHop = recipient;
          delay(100);
          Serial.println("forward paket.");
          sendMessage(incomingData, incomingMsgId, sender, Dst, NextHop);
          LoRa.receive();                     // go back into receive mode 
        } 
    else if (sender == 1 && recipient == 4){
          Dst = 5;
          NextHop = recipient;
          delay(500);
          Serial.println("forward paket.");
          sendMessage(incomingData, incomingMsgId, sender, Dst, NextHop);
          LoRa.receive();                     // go back into receive mode
        }
     else if (sender == 5 && recipient == 1){
          Dst = 1;
          NextHop = recipient;
          delay(500);
          Serial.println("forward paket.");
          sendMessage(incomingData, incomingMsgId, sender, Dst, NextHop);
          LoRa.receive();                     // go back into receive mode
        }
      else if (sender == 5 && recipient == 2){
          Dst = 1;
          NextHop = recipient;
          delay(500);
          Serial.println("forward paket.");
          sendMessage(incomingData, incomingMsgId, sender, Dst, NextHop);
          LoRa.receive();                     // go back into receive mode
        }
        
      }
      else {
        Serial.print(String(NextNode));
        Serial.println(" This message is not for me");
  }
}

