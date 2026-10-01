
#include "DHT.h"

#define DHTPIN 32
#define DHTTYPE DHT22
#define AQPIN 35

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(AQPIN,INPUT);
  Serial.println("DHT11 Test");
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int pollution = analogRead(AQPIN);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" C   ");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" % ");
    
    Serial.print("Pollution: ");
    Serial.println(pollution);

  delay(2000);
}