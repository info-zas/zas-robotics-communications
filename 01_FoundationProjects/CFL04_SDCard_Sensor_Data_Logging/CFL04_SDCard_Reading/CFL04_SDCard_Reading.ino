/*
   ESP32 SD Card File Reader
   Reads contents of /mq135_log.csv
   and prints them to Serial Monitor
   SD wiring: CS=5, MOSI=23, MISO=19, SCK=18
*/

#include <SPI.h>
#include <SD.h>

const int SD_CS = 5;
File myFile;

void setup() {
  Serial.begin(115200);
  delay(500);
  while (!Serial) {;}

  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD initialization done.");

  // Try opening the file
  myFile = SD.open("/dht_log.csv");
  if (myFile) {
    Serial.println("----- File Content Start -----");
    // read from the file until there's nothing else in it
    while (myFile.available()) {
      Serial.write(myFile.read());  // direct raw output
    }
    myFile.close();
    Serial.println("\n----- File Content End -----");
  } else {
    // if the file didn't open, print an error
    Serial.println("error opening /mq135_log.csv");
  }
}

void loop() {
  // Nothing here – runs once
}

