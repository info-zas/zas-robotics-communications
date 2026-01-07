#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === OLED Setup ===
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === nRF24L01 Setup ===
RF24 radio(4, 5); // CE = 4, CSN = 5
const byte address[6] = "00001";

// === Global Variable ===
uint8_t receivedIRCode = 0;

void setup() {
  Serial.begin(9600);

  // === OLED Init ===
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("nRF24 Receiver");
  display.display();

  // === nRF24L01 Init ===
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening(); // Set as receiver
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedIRCode, sizeof(receivedIRCode));

    Serial.print("Received IR Code: 0x");
    Serial.println(receivedIRCode, HEX);

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("IR Code Received:");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print("0x");
    display.println(receivedIRCode, HEX);
    display.display();
  }

  delay(50);
}
