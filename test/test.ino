 void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Node Sensor");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("coba");
  delay(1000);
}
