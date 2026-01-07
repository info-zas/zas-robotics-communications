#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <LoRa.h>

// ========= WIFI =========
const char *ssid = "Enter Your SSID Here";
const char *password = "Enter Your WiFi Password Here";

// ========= LORA PINS =========
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 2

WebServer server(80);

// ========= DATA =========
float loraTemperature = 0.0;

// ========= HTML =========
void handleRoot() {
  char msg[800];

  snprintf(msg, sizeof(msg),
           "<html><head>"
           "<meta http-equiv='refresh' content='5'/>"
           "<meta name='viewport' content='width=device-width, initial-scale=1'>"
           "<title>LoRa Gateway</title>"
           "</head><body style='font-family:Arial;text-align:center;'>"
           "<h2 style='color:green'>LoRa → WiFi Gateway</h2>"
           "<p style='font-size:2rem'>Temperature: <b>%.2f °C</b></p>"
           "</body></html>",
           loraTemperature);

  server.send(200, "text/html", msg);
}

void setup() {
  Serial.begin(115200);

  // ===== FORCE STA MODE =====
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ===== HTTP SERVER =====
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // ===== LORA INIT (AFTER WIFI) =====
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    while (true);
  }

  Serial.println("LoRa Receiver Ready");
}

void loop() {
  server.handleClient();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    loraTemperature = incoming.toFloat();

    Serial.print("Received LoRa Temp: ");
    Serial.println(loraTemperature);
  }

  delay(10);  // IMPORTANT: yield to WiFi task
}
