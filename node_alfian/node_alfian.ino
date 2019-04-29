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
#define node_addr 5
#define dhtPin 3 // Use d3 pin to connect the data line of DHT11, it is up to you
//DHT dht11 (dhtPin,dhtType);
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHDatagram kurir(rf95, node_addr);
// Blinky on receipt
#define LED 13
struct broad {
  uint8_t gtwAddr,cons;
};
struct dataSet
{
  uint8_t ack, id, temp, hum;
};

void setup() {
  // put your setup code here, to run once:
//  dht11.begin();
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  while (!Serial);
  Serial.begin(9600);
  delay(100);
  Serial.println("Lora Node Sensor Humidity and Temperature\n\n");
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
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
  if (kurir.available())
  {
    Serial.println("available message");
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    Serial.println("kurir masuk");
    struct broad auth;
    if (kurir.recvfrom((uint8_t*) &auth, &len, &from))
    {
      // RH_RF95::printBuffer("Received: ", buf, len);
      if (auth.cons == 13)// auth from gateway
      {
        digitalWrite(LED, HIGH);
        // RH_RF95::printBuffer("Received: ", buf, len);
        Serial.println("Got: true gateway");
        struct dataSet data;
        data.ack = 201 ;
        data.id = node_addr ; //Device ID / sensor node
        data.hum = "100"; // store humidity data
        data.temp = "20";// store temperature data
        kurir.setHeaderFrom(node_addr);
        kurir.sendto((uint8_t *) &data, sizeof(struct dataSet), auth.gtwAddr); // Send out Ack + ID + Sensor data to LoRa gateway
        kurir.waitPacketSent();
        Serial.println(from, DEC);
        Serial.println("Sent a reply");
        digitalWrite(LED, LOW);
        Serial.print("Current humidity = ");
        Serial.print((uint8_t)"100", DEC);
        Serial.print("% ");
        Serial.print("temperature = ");
        Serial.print((uint8_t) "20", DEC);
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
