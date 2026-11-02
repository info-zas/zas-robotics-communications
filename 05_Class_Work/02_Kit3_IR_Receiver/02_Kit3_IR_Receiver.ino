#include <IRremote.h>
#define IR_PIN     34
#define LED_PIN 17


void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  IrReceiver.begin(IR_PIN);
  Serial.println("IR Test Ready");
}

void loop() {
  if (IrReceiver.decode()) {

    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("Code: 0x");
    Serial.println(code, HEX);

    // Check for your IR code
    if (code == 0xE619FF00) {
      Serial.println("Button Detected - Beep!");

      digitalWrite(LED_PIN, HIGH);  // 2 kHz tone
      delay(200);
      digitalWrite(LED_PIN,LOW);
    }

    IrReceiver.resume();
  }
}

