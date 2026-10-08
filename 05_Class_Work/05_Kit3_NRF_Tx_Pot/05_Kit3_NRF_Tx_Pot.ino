
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
#define POT 15

int counter = 0;
int lastStateCLK;

struct Data {
  int encoder;
  int pot;
};

Data data;

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
  }

lastStateCLK = currentStateCLK;

// Read both inputs
  data.encoder = counter;
  data.pot = analogRead(POT);

  // Send both values
  radio.write(&data, sizeof(data));
  // Display both values
  display.clearDisplay();
  display.setTextSize(2);

  display.setCursor(0, 5);
  display.print("Enc: ");
  display.println(data.encoder);

  display.setCursor(0, 35);
  display.print("Pot: ");
  display.println(data.pot);

  display.display();

  delay(100);
}

