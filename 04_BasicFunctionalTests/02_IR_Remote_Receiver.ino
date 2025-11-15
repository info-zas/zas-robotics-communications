#include <IRremote.h>  // Use the latest IRremote library

#define IR_RECEIVE_PIN 34  // Signal pin connected to IR receiver OUT pin

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the receiver
  Serial.println("IR Receiver ready. Press any button on the remote...");
}

void loop() {
  if (IrReceiver.decode()) {  // Check if a signal is received
    // Print the HEX code
    Serial.print("Button HEX: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    
    // Or full raw data
    Serial.print("Raw Data: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    IrReceiver.resume();  // Receive the next value
  }
}
