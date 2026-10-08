/* #include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// === NRF24L01 Setup ===
//RF24 radio(9, 10); // CE, CSN (Change if needed)

// --- NRF24L01 Setup ---
#define CE_PIN 4
#define CSN_PIN 5
RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00002";

// === Encoder Pins ===
#define CLK 26
#define DT 27
#define SW 33

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

void setup() {
  Serial.begin(9600);

  // Encoder pins
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLK);

  // NRF24L01
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Encoder + NRF24L01 Transmitter Ready");
}

void loop() {
  // Read encoder
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
      currentDir = "CCW";
    } else {
      counter++;
      currentDir = "CW";
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);

    // Send over nRF24L01
    radio.write(&counter, sizeof(counter));
  }
  lastStateCLK = currentStateCLK;

  // Check button press (optional)
  int btnState = digitalRead(SW);
  if (btnState == LOW && millis() - lastButtonPress > 200) {
    Serial.println("Encoder button pressed");
    lastButtonPress = millis();
    // You could send a special value or reset the counter, etc.
  }

  delay(1);
}*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RF24 radio(4, 5);
const byte address[6] = "00001";

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define CLK 26
#define DT 27

int counter = 0;
int lastStateCLK;

void setup() {
  Serial.begin(9600);

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  lastStateCLK = digitalRead(CLK);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {

  int currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK && currentStateCLK == HIGH) {

    if (digitalRead(DT) != currentStateCLK)
      counter--;
    else
      counter++;

    // Send value
    radio.write(&counter, sizeof(counter));

    // Display value
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print("Value: ");
    display.println(counter);
    display.display();

    Serial.println(counter);
  }

  lastStateCLK = currentStateCLK;
}
