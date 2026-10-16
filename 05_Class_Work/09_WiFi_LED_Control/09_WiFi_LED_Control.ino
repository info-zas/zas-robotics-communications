#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "156HomeKitchen";
const char* WIFI_PASS = "Hitchin786&";

constexpr int LED_PIN = 17;
bool ledOn = false;

WebServer server(80);

void handleRoot() {
  String page = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 LED Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 60px 20px;
      background: #f4f4f4;
    }
    button {
      font-size: 24px;
      padding: 18px 40px;
      margin: 12px;
      border: 0;
      border-radius: 10px;
      color: white;
      cursor: pointer;
    }
    .on { background: #16803c; }
    .off { background: #c62828; }
  </style>
</head>
<body>
  <h1>ESP32 LED Control</h1>
)HTML";

  page += "<h2>LED is ";
  page += ledOn ? "ON" : "OFF";
  page += "</h2>";

  page += R"HTML(
  <form action="/on" method="POST">
    <button class="on" type="submit">ON</button>
  </form>
  <form action="/off" method="POST">
    <button class="off" type="submit">OFF</button>
  </form>
</body>
</html>
)HTML";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", page);
}

void setLed(bool state) {
  ledOn = state;
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Open http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_POST, []() { setLed(true); });
  server.on("/off", HTTP_POST, []() { setLed(false); });

  server.begin();
}

void loop() {
  server.handleClient();
  delay(2);
}
