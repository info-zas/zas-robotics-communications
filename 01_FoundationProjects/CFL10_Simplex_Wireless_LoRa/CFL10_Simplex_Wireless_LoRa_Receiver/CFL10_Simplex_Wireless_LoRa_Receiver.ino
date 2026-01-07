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

void setup() {
  Serial.begin(9600);

  // --- Initialize OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa RX Starting...");
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

  Serial.println("LoRa RX Ready");
  display.setCursor(0, 20);
  display.println("LoRa Ready");
  display.display();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.print("Received: ");
    Serial.println(received);

    // --- Extract Pot Value ---
    int potValue = extractPotValue(received);

    // --- Display on OLED ---
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LoRa RX");
    display.setCursor(0, 20);
    display.print("Pot Value: ");
    display.println(potValue);
    display.display();
  }
}

// --- Helper Function to Extract Pot Value ---
int extractPotValue(const String& message) {
  int idx = message.indexOf(':');
  if (idx != -1) {
    return message.substring(idx + 1).toInt();
  }
  return 0;
}
