//GATEWAY
#include <SPI.h>
#include <RH_RF95.h>
#include <RHDatagram.h>
#include <stdio.h>
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
#define gtw_addr 9
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
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
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  //while (!Serial);
  Serial.begin(9600);
  delay(100);
  Serial.println("Arduino LoRa TX Test!");
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!kurir.init()) {
    Serial.println("kurir init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  Serial.println(kurir.thisAddress());
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
  if (millis() - displayTimer >= 5000) {
    displayTimer = millis();
    Serial.println("Sending broadcast to Node");
    // Send a message to node
    struct broad auth;
    auth.gtwAddr = gtw_addr;
    auth.cons = 14;
    rf95.send((uint8_t *) &auth, sizeof(struct broad));
  }
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  Serial.println("Waiting for reply...");
  if (kurir.waitAvailableTimeout(1000))
  {
    // Should be a reply message for us now
    // Serial.println(buf[0]);
    uint8_t from;
    struct dataSet data;
    if (kurir.recvfrom((uint8_t *)&data, &len, &from))
    {
      if (data.ack == 201)
      {
        Serial.println("Ack node OK");
        Serial.print("Available node : ");
        for (int j = 0; j < (sizeof(avail) / sizeof(avail[0])); ++j)
        {
          if (j == (sizeof(avail) / sizeof(avail[0]) - 1)) {
            for (int a = 0; a < (sizeof(avail) / sizeof(avail[0])); ++a)
            {
              if (avail[a].addr == 0) {
                avail[a].addr = from;
                w[a].t = millis();
                break;
              }
            }
          } else if (avail[j].addr == from ) {
            w[j].t = millis();
            break;
          } else if (avail[j].addr != from ) {
            continue;
          }
        }
        unsigned long param = millis();
        for (int c = 0; c < (sizeof(w)/sizeof(w[0]));++c){
          if ((param-w[c].t)>=50000){
            avail[c].addr = 0;
            w[c].t = 0;
          }
        }
        for (int i = 0; i < (sizeof(avail) / sizeof(avail[0])); ++i)
        {
          if (avail[i].addr != 0) {
            Serial.print(avail[i].addr);
            Serial.print(" ");
          }
        }
        // Serial.print(from);
        Serial.print("\nMendapat data dari node : ");
        Serial.println(from, DEC);
        Serial.print("ID = ");
        Serial.print(data.id);
        Serial.print(" : Current humidity = ");
        Serial.print(data.hum);
        Serial.print("% ");
        Serial.print("temperature = ");
        Serial.print(data.temp);
        Serial.println(" C ");
      }
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
}
