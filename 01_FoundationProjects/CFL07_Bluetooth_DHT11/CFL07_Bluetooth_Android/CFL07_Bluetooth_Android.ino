#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);

  // Start ESP32 Bluetooth
  SerialBT.begin("ZAS_Robotics");

  Serial.println("--------------------------------");
  Serial.println("ESP32 Bluetooth Terminal");
  Serial.println("Waiting for Bluetooth Data...");
  Serial.println("--------------------------------");
}

void loop()
{
  // Phone -> ESP32
  if (SerialBT.available())
  {
    String message = SerialBT.readStringUntil('\n');
    message.trim();

    Serial.println("--------------------------------");
    Serial.print("Received Packet : ");
    Serial.println(message);
    Serial.println("--------------------------------");

    // Send reply back to phone
    SerialBT.print("ACK : ");
    SerialBT.println(message);
  }

  // PC Serial Monitor -> Phone
  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    SerialBT.println(data);
  }

  delay(20);
}