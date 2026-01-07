/*
  ESP32 + DHT11 + OLED + microSD Logger
  - SD (SPI): CS=5, MOSI=23, MISO=19, SCK=18
  - OLED (I2C): SDA=21, SCL=22
  - DHT11: DATA -> GPIO14 (can use any digital pin)
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// -------- OLED Setup --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------- DHT Setup --------
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------- SD Setup --------
const int SD_CS = 5;
const char* LOG_PATH = "/dht_log.csv";
const unsigned long LOG_INTERVAL_MS = 1000; // log every 1 second

File myFile;
unsigned long lastLog = 0;
bool sd_ok = false;

// -------- Helper Functions --------
void writeHeaderIfNeeded() {
  if (!SD.exists(LOG_PATH)) {
    myFile = SD.open(LOG_PATH, FILE_WRITE);
    if (myFile) {
      myFile.println("millis,temperature_C,humidity_%");
      myFile.close();
      Serial.println("Created log file with header.");
    } else {
      Serial.println("Failed to create log file for header.");
    }
  }
}

void appendCSV(unsigned long t_ms, float temp, float hum) {
  myFile = SD.open(LOG_PATH, FILE_APPEND);
  if (myFile) {
    myFile.printf("%lu,%.2f,%.2f\n", t_ms, temp, hum);
    myFile.close();
    Serial.printf("Logged: %lu, %.2f C, %.2f %%\n", t_ms, temp, hum);
  } else {
    Serial.println("Error opening log file for append.");
  }
}

void showOnOLED(float temp, float hum) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DHT11 Sensor");

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.printf("%.1fC", temp);
  display.setCursor(72, 16);
  display.printf("%.0f%%", hum);

  display.setTextSize(1);
  display.setCursor(0, 48);
  if (sd_ok) {
    display.println("SD: OK  logging...");
  } else {
    display.println("SD: FAILED (no logging)");
  }

  display.display();
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  delay(500);

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

  // DHT init
  dht.begin();

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
  display.println("DHT11 Logger");
  display.setCursor(0, 12);
  display.println(sd_ok ? "SD Ready" : "SD Failed");
  display.setCursor(0, 24);
  display.println("OLED Ready");
  display.display();
  delay(1500);
}

// -------- Loop --------
void loop() {
  float temp = dht.readTemperature(); // °C
  float hum = dht.readHumidity();     // %

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Print to Serial
  Serial.printf("Temp: %.2f °C, Humidity: %.2f %%\n", temp, hum);

  // Show on OLED
  showOnOLED(temp, hum);

  // Periodic logging
  unsigned long now = millis();
  if (sd_ok && (now - lastLog >= LOG_INTERVAL_MS)) {
    appendCSV(now, temp, hum);
    lastLog = now;
  }

  delay(200); // fast OLED updates
}


