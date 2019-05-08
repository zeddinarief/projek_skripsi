//NODE
#include <SPI.h>
#include <RH_RF95.h>
#include <RHDatagram.h>
//#include <DHT.h>
//#define dhtType DHT11
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
#define node_addr 4
#define dhtPin 3 // Use d3 pin to connect the data line of DHT11, it is up to you
//DHT dht11 (dhtPin,dhtType);
// Singleton instance of the radio driver
//RH_RF95 rf95(RFM95_CS, RFM95_INT);
RH_RF95 rf95;
RHDatagram kurir(rf95, node_addr);
// Blinky on receipt
#define LED 13
uint32_t displayTimer = 0;
uint8_t no = 0;
struct broad {
  uint8_t gtwAddr,cons;
};
struct dataSet
{
  uint8_t ack, id, temp, hum;
};

void setup() {
  while (!Serial);
  Serial.begin(9600);
  delay(100);
//  Serial.println("Arduino LoRa TX Test!");
  // manual reset
//  digitalWrite(RFM95_RST, LOW);
//  delay(10);
//  digitalWrite(RFM95_RST, HIGH);
//  delay(10);
  while (!rf95.init()) {
    Serial.println("kurir init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
//  Serial.println(kurir.thisAddress());
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - displayTimer >= 5000) {
    displayTimer = millis();
    Serial.print("Sending broadcast to Node ");
    Serial.println(no);
    // Send a message to node
    struct broad auth;
    auth.gtwAddr = 4;
    auth.cons = 13;
    struct dataSet data;
    data.ack = 201 ;
    data.id = node_addr ; //Device ID / sensor node
    data.hum = no; // store humidity data
    data.temp = 20;// store temperature data
    rf95.send((uint8_t *) &data, sizeof(struct dataSet));
//    kurir.setHeaderFrom(node_addr);
//    kurir.sendto((uint8_t *) &data, sizeof(struct dataSet), auth.gtwAddr);
//    kurir.waitPacketSent();
    no++;
  }
  
}
