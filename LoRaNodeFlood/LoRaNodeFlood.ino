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
//#include <DHT.h>

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
//DHT dht(3, DHT11);

String outgoing;              // outgoing message

byte msgCount = 1;            // count of outgoing messages

//byte localAddress = 0xBB;     // address of this device
//byte destination = 0xFF;      // destination to send to
byte localAddress = 3;     // address of this device
byte destination = 2;      // destination to send to
byte suhu;
//byte destination = 5;      // destination coba
//byte sensor;
byte currentMsgId = 0;
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
//struct dataSet
//{
//  byte ack;
//  byte sensor;
//};

void setup() {
  Serial.begin(9600);                   // initialize serial
//  dht.begin();
  while (!Serial);

  Serial.println("LoRa Node Sensor");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.print("LoRa init succeeded. Lora node ");
  Serial.println(localAddress);
}

void loop() {
//  suhu = dht.readTemperature();
  suhu = 100;
//  if (millis() - lastSendTime > interval) {
////    String message = "HeLoRa World!";   // send a message
////    byte suhu = dht.readTemperature();
//    Serial.print("suhu: ");
//    Serial.print(suhu);
//    Serial.println(" C");
//    sendMessage(destination, msgCount, suhu);
////    Serial.println("Sending " + message);
//    lastSendTime = millis();            // timestamp the message
//    interval = random(2000) + 1000;    // 2-3 seconds
//  }

//   parse for a packet, and call onReceive with the result:
//  onReceive(LoRa.parsePacket());
//  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void sendMessage(byte dest, byte id, byte sensor) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(dest);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(id);                 // add message ID
  LoRa.write(1);                 // add message type
  LoRa.write(sensor);
//  LoRa.write(outgoing.length());        // add payload length
//  LoRa.print(outgoing);                 // add payload
//  LoRa.write(outgoing.length());        // add payload length
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                  // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingMsgType = LoRa.read();     // incoming msg type
  byte incomingData = LoRa.read();      // incoming data sensor

  Serial.println("Receiving request ....");

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress || incomingMsgType != 0 || incomingMsgId == currentMsgId) {
    Serial.print("This message id : ");
    Serial.print(incomingMsgId);
    Serial.println(" is not for me.");
    Serial.println("");
    return;                             // skip rest of function
  }

  currentMsgId = incomingMsgId;
//  String message = "HeLoRa World!";   // send a message
//  suhu = 100;
//  byte suhu = dht.readTemperature();
  Serial.print("suhu: ");
  Serial.print(suhu);
  Serial.println(" C");
  sendMessage(sender, incomingMsgId, suhu);
  Serial.println("Sending response ...");
//  LoRa.receive();
  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: " + String(sender, DEC));
  Serial.println("Sent to: " + String(recipient, DEC));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message type: request");
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

