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

// Hardware pins
#define POT_PIN      15
#define BUTTON_PIN   25
#define ENCODER_CLK  26
#define ENCODER_DT   27
#define ENCODER_SW   33

int encoderCount = 0;
int lastClk = HIGH;

struct DataPacket {
  int potValue;
  int encoderCount;
  bool buttonPressed;
};
DataPacket myData;
DataPacket receivedData;

void setup() {
  Serial.begin(115200);

  pinMode(POT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("ESP32-A Starting...");
  display.display();
  delay(1000);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(addressAtoB);
  radio.openReadingPipe(1, addressBtoA);
  radio.startListening();
}

void loop() {
  // Track encoder
  int clk = digitalRead(ENCODER_CLK);
  if (clk != lastClk && clk == LOW) {
    if (digitalRead(ENCODER_DT) != clk) {
      encoderCount++;
    } else {
      encoderCount--;
    }
  }
  lastClk = clk;

  // Check button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    myData.potValue = analogRead(POT_PIN);
    myData.encoderCount = encoderCount;
    myData.buttonPressed = true;

    // Send data
    radio.stopListening();
    bool sent = radio.write(&myData, sizeof(myData));
    Serial.println(sent ? "Data sent." : "Send failed.");
    delay(50);

    // Wait for reply
    radio.startListening();
    unsigned long startTime = millis();
    bool gotReply = false;
    while (millis() - startTime < 500) {
      if (radio.available()) {
        radio.read(&receivedData, sizeof(receivedData));
        gotReply = true;
        break;
      }
    }

    // Update OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Pot: ");
    display.println(myData.potValue);
    display.print("Enc: ");
    display.println(myData.encoderCount);
    display.print("Sent!");
    display.setCursor(0, 40);
    if (gotReply) {
      display.println("Reply: OK");
    } else {
      display.println("No reply");
    }
    display.display();

    delay(1000);
  }

  delay(50);
}

