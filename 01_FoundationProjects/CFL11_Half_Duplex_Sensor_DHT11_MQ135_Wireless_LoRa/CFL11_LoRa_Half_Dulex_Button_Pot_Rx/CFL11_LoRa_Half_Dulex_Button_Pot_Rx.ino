#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- LoRa SX1278 SPI Pins ---
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  22

// --- I/O Pins ---
#define LED_PIN     17
#define BUZZER_PIN  16
#define BUTTON_PIN  25
#define POT_PIN     15

// --- Variables ---
bool lastButtonState = HIGH;  // Button unpressed (pull-up)
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Half-Duplex");
  display.display();

  // Pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);

  // LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  delay(1000);  // Let LoRa chip settle

  if (!LoRa.begin(433E6)) {
    display.println("LoRa init failed!");
    display.display();
    while (true);
  }

  display.println("LoRa ready");
  display.display();
}

void loop() {
  // --- Check for Button Press ---
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Debounce
    if ((millis() - lastDebounceTime) > debounceDelay) {
      sendMessage();
      lastDebounceTime = millis();
    }
  }
  lastButtonState = currentButtonState;

  // --- Check for Incoming Message ---
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    displayMessage(incoming);
    notifyUser();
  }
}

// === Function to Send Message ===
void sendMessage() {
  int potValue = analogRead(POT_PIN);  // 0–4095
  String message = "POT:" + String(potValue);

  Serial.println("Sending: " + message);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("You Sent:");
  display.println(message);
  display.display();
}

// === Display Incoming Message ===
void displayMessage(String msg) {
  Serial.println("Received: " + msg);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Msg Received:");
  display.println(msg);
  display.display();
}

// === Flash LED & Buzzer ===
void notifyUser() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

