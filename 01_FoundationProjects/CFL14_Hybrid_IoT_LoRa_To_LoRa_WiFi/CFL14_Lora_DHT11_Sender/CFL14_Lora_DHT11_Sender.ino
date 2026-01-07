#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

// ===== DHT =====
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== LoRa Pins (change if needed) =====
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 2

void setup() {
  Serial.begin(115200);
  dht.begin();

  // LoRa init
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    while (1);
  }

  Serial.println("LoRa Sender Ready");
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  Serial.print("Sending temperature: ");
  Serial.println(temp);

  LoRa.beginPacket();
  LoRa.print(temp);
  LoRa.endPacket();

  delay(3000);  // Send every 3 seconds
}
