#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- I/O ---
#define POT_PIN     15
#define BUTTON_PIN  25
#define LED_PIN     17
#define BUZZER_PIN  16

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

bool lastButton = HIGH;
unsigned long debounceTime = 0;

void setup() {
  Serial.begin(9600);

  // Pins
  pinMode(POT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED fail");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Node B: RX");
  display.display();

  // LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  delay(1000);
  if (!LoRa.begin(433E6)) {
    display.println("LoRa fail");
    display.display();
    while (true);
  }
  display.println("LoRa OK");
  display.display();
}

void loop() {
  // Listen for messages from Node A
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String msg = "";
    while (LoRa.available()) msg += (char)LoRa.read();
    displayIncoming(msg);
    alertUser();
  }

  // Button-triggered reply
  bool btn = digitalRead(BUTTON_PIN);
  if (lastButton == HIGH && btn == LOW && millis() - debounceTime > 100) {
    debounceTime = millis();
    sendPotValue();
  }
  lastButton = btn;
}

void displayIncoming(String msg) {
  Serial.println("Received: " + msg);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Data from Node A:");
  display.println(msg);
  display.display();
}

void sendPotValue() {
  int pot = analogRead(POT_PIN);
  String reply = "Pot:" + String(pot);
  Serial.println("Sending: " + reply);

  LoRa.beginPacket();
  LoRa.print(reply);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("You Sent:");
  display.println(reply);
  display.display();
}

void alertUser() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

