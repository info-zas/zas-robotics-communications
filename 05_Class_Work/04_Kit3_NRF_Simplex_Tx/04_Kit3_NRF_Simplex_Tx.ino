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
