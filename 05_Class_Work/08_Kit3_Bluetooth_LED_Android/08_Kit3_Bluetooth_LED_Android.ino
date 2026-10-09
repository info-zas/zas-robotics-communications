#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define LED_PIN 17

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  SerialBT.begin("ZAS_Robotics");

  Serial.println("Bluetooth Ready");
  Serial.println("Send ON or OFF from phone");
}

void loop()
{
  if (SerialBT.available())
  {
    String message = SerialBT.readStringUntil('\n');
    message.trim();
    message.toUpperCase();

    Serial.print("Received: ");
    Serial.println(message);

    if (message == "ON")
    {
      digitalWrite(LED_PIN, HIGH);
      SerialBT.println("LED ON");
      Serial.println("LED ON");
    }

    else if (message == "OFF")
    {
      digitalWrite(LED_PIN, LOW);
      SerialBT.println("LED OFF");
      Serial.println("LED OFF");
    }
  }
}