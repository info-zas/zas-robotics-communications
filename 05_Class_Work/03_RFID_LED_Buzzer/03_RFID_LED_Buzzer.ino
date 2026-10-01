#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- RFID ---
#define SS_PIN 5
#define RST_PIN 13

// --- LED and Buzzer ---
#define LED_PIN 17
#define BUZZER_PIN 16

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);

  // Start SPI and RFID
  SPI.begin();
  mfrc522.PCD_Init();

  // Start OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Scan RFID...");
  display.display();

  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  // Show UID on Serial Monitor
  Serial.print("UID tag :");

  String content = "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);

    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");

  content.toUpperCase();

  // Show UID on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("RFID UID:");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(content.substring(1));

  display.display();

  // FOB 1 - LED
  if (content.substring(1) == "61 27 5D A4")
  {
    Serial.println("LED ON");

    digitalWrite(LED_PIN, HIGH);
    delay(3000);
    digitalWrite(LED_PIN, LOW);
  }

  // FOB 2 - Buzzer
  else if (content.substring(1) == "82 36 B4 00")
  {
    Serial.println("BUZZER ON");

    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000);
    digitalWrite(BUZZER_PIN, LOW);
  }

  else
  {
    Serial.println("Access denied");
    delay(3000);
  }
}