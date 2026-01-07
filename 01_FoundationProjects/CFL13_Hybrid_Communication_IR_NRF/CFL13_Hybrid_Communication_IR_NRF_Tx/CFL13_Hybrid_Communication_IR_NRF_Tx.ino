#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === Pin Configuration ===
#define IR_RECEIVE_PIN 34
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// === OLED Display Setup ===
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === nRF24L01 Setup ===
RF24 radio(4, 5);  // CE = 4, CSN = 5
const byte address[6] = "00001";

// === Global Variables ===
uint8_t irCode = 0;

void setup() {
  Serial.begin(9600);

  // === OLED Init ===
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("IR + nRF Sender");
  display.display();
  delay(1000);

  // === IR Init ===
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready...");

  // === nRF24L01 Init ===
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening(); // Set as transmitter
}

void loop() {
  if (IrReceiver.decode()) {
    // Get command (only the lower 8 bits of IR command)
    irCode = IrReceiver.decodedIRData.command;

    Serial.print("IR Code: 0x");
    Serial.println(irCode, HEX);

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("IR Code Received:");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print("0x");
    display.println(irCode, HEX);
    display.display();

    // Send via nRF24L01
    radio.write(&irCode, sizeof(irCode));
    Serial.println("IR Code sent over RF");

    IrReceiver.resume(); // Ready to receive next
  }

  delay(10); // debounce delay
}
