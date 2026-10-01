#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
  display.println("LoRa Receiver");
  display.display();

  // LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa failed!");

    display.println("LoRa failed!");
    display.display();

    while (true);
  }

  Serial.println("LoRa Receiver Ready");

  display.println("LoRa OK");
  display.println("Waiting...");
  display.display();
}

void loop() {

  // Check for LoRa packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {

    String message = "";

    // Read received message
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    // Serial Monitor
    Serial.print("Received: ");
    Serial.println(message);

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("LoRa Received:");
    display.println();
    display.println(message);

    display.display();
  }
}
