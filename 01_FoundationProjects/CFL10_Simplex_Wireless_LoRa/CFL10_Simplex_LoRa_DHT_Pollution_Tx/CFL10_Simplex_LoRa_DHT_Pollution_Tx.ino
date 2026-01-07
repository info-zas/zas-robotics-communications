#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// --- LoRa SX1278 Pin Definitions ---
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

// --- OLED Definitions ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- DHT Sensor ---
#define DHTPIN 32       // DHT sensor connected to GPIO 32
#define DHTTYPE DHT11   // Change to DHT22 if using that sensor
DHT dht(DHTPIN, DHTTYPE);

// --- MQ-135 Sensor ---
#define MQ135_PIN 35    // MQ-135 analog output connected to GPIO 35

void setup() {
  Serial.begin(9600);
  pinMode(MQ135_PIN, INPUT);

  // --- Initialize OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa TX Starting...");
  display.display();

  // --- Reset and Init LoRa ---
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(100);
  digitalWrite(LORA_RST, HIGH);
  delay(100);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    display.setCursor(0, 20);
    display.println("LoRa Failed!");
    display.display();
    while (true);
  }

  // --- Initialize DHT ---
  dht.begin();

  Serial.println("LoRa TX Ready");
  display.setCursor(0, 20);
  display.println("LoRa Ready");
  display.display();
  delay(1000);
}

void loop() {
  // --- Read Sensors ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int mq135Value = analogRead(MQ135_PIN);

  // --- Check for DHT errors ---
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT reading failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("DHT Error!");
    display.display();
    delay(2000);
    return;
  }

  // --- Create message for LoRa transmission ---
  String message = "Temp:" + String(temperature, 1) + "C  Hum:" + String(humidity, 1) +
                   "%  MQ135:" + String(mq135Value);

  // --- Send over LoRa ---
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  // --- Serial Output ---
  Serial.print("Sent -> ");
  Serial.println(message);

  // --- Display on OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LoRa TX");
  display.setCursor(0, 15);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");
  display.setCursor(0, 30);
  display.print("Hum:  ");
  display.print(humidity, 1);
  display.println(" %");
  display.setCursor(0, 45);
  display.print("MQ135: ");
  display.println(mq135Value);
  display.display();

  delay(5000);  // Transmit every 5 seconds
}
