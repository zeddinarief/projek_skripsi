
#include <SPI.h>              // include libraries
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte MsgID = 1;            // count of outgoing messages
byte NodeID = 1;     // address of this device
byte Src = 1;
byte Dst = 0;      // destination to send to
byte NextHop = 0;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  
  Serial.println("LoRa Gateway");
  Serial.println(sizeof(LoRa));
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded.\nId : ");
  Serial.println(localAddress);
}


void loop() {
  String in = Serial.read();
  if (in == "r1"){    
    Dst = 5;
    NextHop = 3;    
    sendMessage();
    Serial.println("Sending request to sensor  ");    
  }
  else if (in == "r2"){
    Dst = 4;
    NextHop = 2;    
    sendMessage();
    Serial.println("Sending request to sensor  ");        
    }
  else if (in == "r3")
    Dst = 3;
    NextHop = 2;    
    sendMessage();
    Serial.println("Sending request to sensor  ");    
    }
  LoRa.receive();
    delay(100);
}

void sendMessage() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(Src);              // add destination address
  LoRa.write(Dst);             // add sender address
  LoRa.write(NextHop);             // add sender address
  LoRa.write(MsgID);                 // add message ID
  LoRa.write(0);
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}



void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  byte sender = LoRa.read();          // sender address
  byte recipient = LoRa.read();         // recipient address    
  byte NextNode = LoRa.read();            // Next address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingData = LoRa.read();      // incoming data sensor
  Serial.print("Dikirim ke : ");
  Serial.println(recipient);
  Serial.println("------------------------\n");
  // if the recipient isn't this device or broadcast,
  if (recipient != NodeID) {
    Serial.println("This message is not for me.");
                              // skip rest of function
  }

 else {
    Serial.println("\nReceive respon from: " + String(sender, DEC));
    Serial.println("Sent to: " + String(recipient, DEC));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("data sensor: " + String(incomingData));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();

//    sendMessage("");
//    Replay(destination);
    
  }
}
