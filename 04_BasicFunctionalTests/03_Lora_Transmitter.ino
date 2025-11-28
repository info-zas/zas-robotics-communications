#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- LoRa SX1278 Pin Definitions ---
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

// --- OLED Definitions ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Potentiometer Pin ---
#define POT_PIN 15

void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);

  // --- Initialize OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
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
    while (true);
  }

  Serial.println("LoRa TX Ready");
  display.setCursor(0, 20);
  display.println("LoRa Ready");
  display.display();
}

void loop() {
  int potValue = analogRead(POT_PIN);
  String message = "PotValue:" + String(potValue);

  // --- Send over LoRa ---
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  Serial.print("Sent: ");
  Serial.println(message);

  // --- Display on OLED ---
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LoRa TX");
  display.setCursor(0, 20);
  display.print("Sent: ");
  display.println(potValue);
  display.display();

  delay(2000);  // Send every 2 seconds
}
