#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NRF24L01
RF24 radio(4, 5);  // CE, CSN
const byte addressAtoB[6] = "Node1";
const byte addressBtoA[6] = "Node2";

// Outputs
#define LED_PIN     17
#define BUZZER_PIN  16

struct DataPacket {
  int potValue;
  int encoderCount;
  bool buttonPressed;
};
DataPacket receivedData;
DataPacket replyData;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("ESP32-B Starting...");
  display.display();
  delay(1000);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(addressBtoA);
  radio.openReadingPipe(1, addressAtoB);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    Serial.println("Data received.");

    // Action based on pot value
    if (receivedData.potValue > 3000) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }

    // Show on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("From A:");
    display.setCursor(0, 15);
    display.print("Pot: ");
    display.println(receivedData.potValue);
    display.print("Enc: ");
    display.println(receivedData.encoderCount);
    display.display();

    delay(200);  // Small pause before reply

    // Send back confirmation
    replyData.potValue = 0;
    replyData.encoderCount = 0;
    replyData.buttonPressed = false;

    radio.stopListening();
    bool success = radio.write(&replyData, sizeof(replyData));
    Serial.println(success ? "Reply sent." : "Reply failed.");
    radio.startListening();
  }

  delay(100);
}
