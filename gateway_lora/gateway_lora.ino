//GATEWAY
#include <SPI.h>
#include <RH_RF95.h>
#include <RHDatagram.h>
#include <stdio.h>
//#define RFM95_CS 10
//#define RFM95_RST 9
//#define RFM95_INT 2
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
#define gtw_addr 9
// Singleton instance of the radio driver
//RH_RF95 rf95(RFM95_CS, RFM95_INT);
RH_RF95 rf95;
RHDatagram kurir(rf95, gtw_addr);
uint32_t displayTimer = 0;
struct broad {
  uint8_t gtwAddr, cons;
};
struct dataSet
{
  uint8_t ack;
  uint8_t id;
  uint8_t temp;
  uint8_t hum;
};
struct nodes {
  uint8_t addr;
};
struct waktu {
  unsigned long t;
};
struct nodes avail[10] = {};
struct waktu w[10] = {};

void setup() {
  while (!Serial);
  Serial.begin(9600);
  delay(100);
  Serial.println("Lora Node Sensor Humidity and Temperature\n\n");
  // manual reset
//  digitalWrite(RFM95_RST, LOW);
//  delay(10);
//  digitalWrite(RFM95_RST, HIGH);
//  delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (kurir.available())
  {
    Serial.println("available message");
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    Serial.println("kurir masuk");
    struct broad auth;
    struct dataSet data;
    if (kurir.recvfrom((uint8_t*) &data, &len, &from))
    {
      // RH_RF95::printBuffer("Received: ", buf, len);
      if (from == 4)// auth from gateway
      {
//        digitalWrite(LED, HIGH);
        // RH_RF95::printBuffer("Received: ", buf, len);
//        uint8_t no = 0;
        Serial.println("Got: true gateway");
//        struct dataSet data;
//        data.ack = 201 ;
//        data.id = node_addr ; //Device ID / sensor node
//        data.hum = no; // store humidity data
//        data.temp = 20;// store temperature data
//        kurir.setHeaderFrom(node_addr);
//        kurir.sendto((uint8_t *) &data, sizeof(struct dataSet), auth.gtwAddr); // Send out Ack + ID + Sensor data to LoRa gateway
//        kurir.waitPacketSent();
        Serial.print("\nMendapat data dari node : ");
//        Serial.println(from, DEC);
        Serial.print("ID = ");
        Serial.print(data.id);
        Serial.print(" : Current humidity = ");
        Serial.print(data.hum);
        Serial.print("% ");
        Serial.print("temperature = ");
        Serial.print(data.temp);
        Serial.println(" C ");
      } else {
        Serial.println("gateway belum diketahui");
      }
    }
    else
    {
      Serial.println("Receive failed");
    }
  }

}
