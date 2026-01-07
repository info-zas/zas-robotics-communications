#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === OLED Settings ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // No reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === IR Remote Pin ===
#define IR_RECEIVE_PIN 34

// Function to display message on OLED
void showOnOLED(String label, String value) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(label);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(value);
  display.display();
}

void setup() {
  Serial.begin(9600);

  // === IR Receiver Init ===
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver ready...");

  // === OLED Init ===
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  showOnOLED("Waiting for", "IR input");
}

void loop() {
  if (IrReceiver.decode()) {
    uint8_t cmd = IrReceiver.decodedIRData.command;

    // Print raw command
    Serial.print("IR Code: 0x");
    Serial.println(cmd, HEX);

    // Match command to action
    String action = "Unknown";
    switch (cmd) {
      case 0x45:
        action = "Power";
        break;
      case 0x46:
        action = "Vol+";
        break;
      case 0x47:
        action = "Func/Stop";
        break;
      case 0x44:
        action = "Rewind";
        break;
      case 0x40:
        action = "Play/Pause";
        break;
      case 0x43:
        action = "FastFwd";
        break;
      case 0x07:
        action = "Down";
        break;
      case 0x15:
        action = "Up";
        break;
      case 0x09:
        action = "EQ";
        break;
      // Add more button mappings here as needed
    }

    // Show on OLED
    showOnOLED("Button", action + " (" + String(cmd, HEX) + ")");

    // Resume IR receiver
    IrReceiver.resume();
  }
}
