#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- DHT11 ---
#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- MQ135 ---
#define MQ135_PIN 35

// --- I/O ---
#define BUTTON_PIN 25
#define LED_PIN 17
#define BUZZER_PIN 16

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
  display.println("Node A: TX");
  display.display();

  // DHT
  dht.begin();

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
  bool btn = digitalRead(BUTTON_PIN);

  if (lastButton == HIGH && btn == LOW && millis() - debounceTime > 100) {
    debounceTime = millis();
    sendSensorData();
  }
  lastButton = btn;

  // Also listen for replies from Node B
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String reply = "";
    while (LoRa.available()) reply += (char)LoRa.read();
    displayReceived(reply);
    alertUser();
  }
}

void sendSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int airQuality = analogRead(MQ135_PIN);

  String message = "Temp:" + String(temp, 1) + "C Hum:" + String(hum, 1) + "% MQ135:" + String(airQuality);
  Serial.println("Sending: " + message);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Sent:");
  display.println(message);
  display.display();
}

void displayReceived(String msg) {
  Serial.println("Received: " + msg);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Reply from Node B:");
  display.println(msg);
  display.display();
}

void alertUser() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

