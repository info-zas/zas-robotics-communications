#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(4);

#define LED_PIN 17

void setup() {
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(9600);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Turn LED ON
  digitalWrite(LED_PIN, HIGH);
  display.clearDisplay();
  display.setCursor(10, 25);
  display.print("LED ON !!");
  Serial.println("LED ON");
  display.display();
  delay(1000);

  // Turn LED OFF
  digitalWrite(LED_PIN, LOW);
  display.clearDisplay();
  display.setCursor(10, 25);
  display.print("LED OFF !");
  Serial.println("LED OFF");
  display.display();
  delay(1000);
}

