#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
#define BUZZER_PIN   16
#define ENCODER_SW   33
#define ENCODER_CLK  26
#define ENCODER_DT   27

int lastClkState = HIGH;
int encoderCount = 0;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);

  digitalWrite(BUZZER_PIN, LOW);

  // Start I2C on ESP32 default SDA/SCL (21, 22)
  Wire.begin(21, 22);

  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true); // halt if OLED not found
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Rotary Init");
  display.display();

  delay(1000);  // Short welcome pause
}

void loop() {
  // --- Check button press ---
  if (digitalRead(ENCODER_SW) == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // --- Check rotation ---
  int clkState = digitalRead(ENCODER_CLK);
  if (clkState != lastClkState && clkState == LOW) {
    if (digitalRead(ENCODER_DT) != clkState) {
      encoderCount++;  // Clockwise
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
    } else {
      encoderCount--;  // Counter-clockwise
    }

    // Update OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Encoder:");
    display.setCursor(0, 30);
    display.println(encoderCount);
    display.display();
  }
  lastClkState = clkState;

  delay(10); // debounce
}
