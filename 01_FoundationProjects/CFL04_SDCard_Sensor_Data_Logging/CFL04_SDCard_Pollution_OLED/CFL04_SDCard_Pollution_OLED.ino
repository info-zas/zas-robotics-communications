/*
  ESP32 + MQ-135 + OLED + microSD Logger
  - SD (SPI): CS=5, MOSI=23, MISO=19, SCK=18
  - OLED (I2C): SDA=21, SCL=22 (default ESP32)
  - MQ-135 analog out -> GPIO14 (ADC2_CH6)
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------- OLED Setup --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------- Pins --------
const int SD_CS = 5;
const int MQ135_PIN = 32;  // Consider moving to ADC1 pin if you use Wi-Fi (e.g., 32)

// -------- Logging --------
const char* LOG_PATH = "/mq135_log.csv";
const unsigned long LOG_INTERVAL_MS = 1000; // log every 1 second

File myFile;
unsigned long lastLog = 0;
bool sd_ok = false;

void writeHeaderIfNeeded() {
  if (!SD.exists(LOG_PATH)) {
    myFile = SD.open(LOG_PATH, FILE_WRITE);
    if (myFile) {
      myFile.println("millis,raw_value");
      myFile.close();
      Serial.println("Created log file with header.");
    } else {
      Serial.println("Failed to create log file for header.");
    }
  }
}

void appendCSV(unsigned long t_ms, int raw) {
  myFile = SD.open(LOG_PATH, FILE_APPEND); // FILE_WRITE also appends on ESP32; FILE_APPEND is explicit if available
  if (myFile) {
    myFile.printf("%lu,%d\n", t_ms, raw);
    myFile.close();
    Serial.print("Logged: ");
    Serial.print(t_ms);
    Serial.print(", ");
    Serial.println(raw);
  } else {
    Serial.println("Error opening log file for append.");
  }
}

void showOnOLED(int value) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MQ-135 Sensor");

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(value);

  display.setTextSize(1);
  display.setCursor(0, 48);
  if (sd_ok) {
    display.println("SD: OK  logging...");
  } else {
    display.println("SD: FAILED (no logging)");
  }

  display.display();
}

void setup() {
  // Serial
  Serial.begin(115200);
  delay(500);
  while (!Serial) {;}

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // SD init
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD initialization failed!");
    sd_ok = false;
  } else {
    Serial.println("SD initialization done.");
    sd_ok = true;
    writeHeaderIfNeeded();
  }

  // Initial splash
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MQ-135 Logger");
  display.setCursor(0, 12);
  display.println(sd_ok ? "SD Ready" : "SD Failed");
  display.setCursor(0, 24);
  display.println("OLED Ready");
  display.display();
  delay(1500);
}

void loop() {
  // Read sensor (0-4095 default 12-bit)
  int sensorValue = analogRead(MQ135_PIN);

  // Print to Serial
  Serial.print("MQ-135 Raw Value: ");
  Serial.println(sensorValue);

  // Show on OLED
  showOnOLED(sensorValue);

  // Periodic logging
  unsigned long now = millis();
  if (sd_ok && (now - lastLog >= LOG_INTERVAL_MS)) {
    appendCSV(now, sensorValue);
    lastLog = now;
  }

  delay(200); // update OLED quickly; logging interval is controlled by LOG_INTERVAL_MS
}

