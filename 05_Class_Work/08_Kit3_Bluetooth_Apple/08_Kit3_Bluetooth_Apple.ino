#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define LED_PIN 17

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHAR_UUID    "abcd1234-1234-1234-1234-abcdef123456"

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String message = pCharacteristic->getValue();

    message.trim();
    message.toUpperCase();

    Serial.print("Received: ");
    Serial.println(message);

    if (message == "ON")
    {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    }

    else if (message == "OFF")
    {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    }
  }
};

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Start Bluetooth
  BLEDevice::init("ZAS_Robotics");

  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService =
      pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic =
      pService->createCharacteristic(
          CHAR_UUID,
          BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE
      );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising =
      BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Bluetooth Ready");
  Serial.println("Connect using iPhone");
}

void loop()
{
  delay(100);
}