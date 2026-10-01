#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- DHT22 ---
#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- MQ135 ---
#define MQ135_PIN 35

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- LoRa Pins ---
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  22

void setup() {
  Serial.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LoRa Transmitter");
  display.display();

  // DHT22
  dht.begin();

  // LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa failed!");

    display.println("LoRa failed!");
    display.display();

    while (true);
  }

  Serial.println("LoRa OK");

  display.println("LoRa OK");
  display.display();

  delay(2000);
}

void loop() {

  // Read sensors
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int airQuality = analogRead(MQ135_PIN);

  // Create message
  String message = "Temp:" + String(temp, 1) +
                   "C Hum:" + String(hum, 1) +
                   "% MQ135:" + String(airQuality);

  // Send through LoRa
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  // Serial Monitor
  Serial.println("Sending:");
  Serial.println(message);

  // OLED
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("Sending LoRa:");
  display.print("Temp: ");
  display.print(temp, 1);
  display.println(" C");

  display.print("Hum:  ");
  display.print(hum, 1);
  display.println(" %");

  display.print("MQ135: ");
  display.println(airQuality);

  display.display();

  // Send every 2 seconds
  delay(2000);
}