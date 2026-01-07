#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- RFID ----------
#define SS_PIN 5
#define RST_PIN 13
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  // I2C + OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID Ready");
  display.display();

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Read UID
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    content += String(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();

  display.clearDisplay();
  display.setCursor(0, 0);

  // Check authorization
  if (content.substring(1) == "61 27 5D A4" ||
      content.substring(1) == "82 36 B4 00") {

    display.println("ACCESS");
    display.println("AUTHORIZED");
    Serial.println("Authorized access");

  } else {
    display.println("ACCESS");
    display.println("DENIED");
    Serial.println("Access denied");
  }

  display.display();
  delay(3000);

  // Clear screen after delay
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Scan Card...");
  display.display();
}

