#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LED pin
#define LED_PIN 17

void setup() {
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);

  // Initialize I2C
  Wire.begin(21, 22);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Turn LED ON
  digitalWrite(LED_PIN, HIGH);
  display.clearDisplay();
  display.setCursor(10, 25);
  display.print("LED ON !!");
  display.display();
  delay(1000);

  // Turn LED OFF
  digitalWrite(LED_PIN, LOW);
  display.clearDisplay();
  display.setCursor(10, 25);
  display.print("LED OFF !");
  display.display();
  delay(1000);
}
