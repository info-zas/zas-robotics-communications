#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RF24 radio(4, 5);
const byte address[6] = "00001";

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Same structure as transmitter
struct Data {
  int encoder;
  int pot;
};

Data data;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {

  if (radio.available()) {

    // Receive both values
    radio.read(&data, sizeof(data));

    // Serial Monitor
    Serial.print("Encoder: ");
    Serial.print(data.encoder);
    Serial.print("  Pot: ");
    Serial.println(data.pot);

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
  }
}
