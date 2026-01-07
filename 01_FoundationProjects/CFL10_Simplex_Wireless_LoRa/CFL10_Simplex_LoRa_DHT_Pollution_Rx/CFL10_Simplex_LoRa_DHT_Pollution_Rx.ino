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
  display.setTextSize(1);
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
    display.setCursor(0, 20);
    display.println("LoRa Failed!");
    display.display();
    while (true);
  }

  Serial.println("LoRa RX Ready");
  display.setCursor(0, 20);
  display.println("LoRa Ready");
  display.display();
  delay(1000);
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

    // --- Extract temperature, humidity, and MQ135 values ---
    float temperature = extractValue(received, "Temp:");
    float humidity = extractValue(received, "Hum:");
    int mq135Value = (int)extractValue(received, "MQ135:");

    // --- Display on OLED ---
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("LoRa RX");

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
    display.print(mq135Value);
    display.display();

    // --- Serial Monitor Output ---
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" C | Humidity: ");
    Serial.print(humidity, 1);
    Serial.print(" % | MQ135: ");
    Serial.println(mq135Value);

    // Optional: Print signal strength
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}

// --- Helper Function to Extract Values ---
float extractValue(const String& message, const String& label) {
  int startIdx = message.indexOf(label);
  if (startIdx == -1) return NAN;
  startIdx += label.length();

  int endIdx = message.indexOf(' ', startIdx);
  if (endIdx == -1) endIdx = message.length();

  return message.substring(startIdx, endIdx).toFloat();
}



