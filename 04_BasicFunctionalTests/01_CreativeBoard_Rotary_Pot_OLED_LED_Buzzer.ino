#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Rotary Encoder
#define CLK 26
#define DT 27
#define SW 33

// Other Components
#define POT_PIN 15
#define LED_PIN 17
#define BUZZER_PIN 16

volatile int counter = 0;
String currentDir = "STOP";
unsigned long lastButtonPress = 0;

// Interrupt handler
void IRAM_ATTR readEncoder() {
  int dtState = digitalRead(DT);
  if (dtState == digitalRead(CLK)) {
    counter++;
    currentDir = "CW";
  } else {
    counter--;
    currentDir = "CCW";
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(CLK), readEncoder, RISING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();
  delay(1000);
}

void loop() {
  // Potentiometer controls LED brightness
  int potValue = analogRead(POT_PIN);
  int ledValue = map(potValue, 0, 4095, 0, 255);
  analogWrite(LED_PIN, ledValue);

  // Button press -> buzzer + message
  if (digitalRead(SW) == LOW) {
    if (millis() - lastButtonPress > 300) {
      tone(BUZZER_PIN, 1000, 150);
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("Button!");
      display.display();
      delay(200);
    }
    lastButtonPress = millis();
  }

  // Display all info
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Dir: ");
  display.println(currentDir);
  display.print("Count: ");
  display.println(counter);
  display.print("Pot: ");
  display.println(potValue);
  display.display();

  delay(50);
}
