#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "roohi";
const char* WIFI_PASS = "Hitchin786&";

constexpr int LED_PIN = 17;
bool ledOn = false;

// ★ BUZZER: pin and state
constexpr int BUZZER_PIN = 16;
bool buzzerOn = false;

WebServer server(80);

void handleRoot() {
  String page = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 LED and Buzzer Control</title>
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
  <h1>ESP32 Control Panel</h1>
)HTML";

  page += "<h2>LED is ";
  page += ledOn ? "ON" : "OFF";
  page += "</h2>";

  page += R"HTML(
  <form action="/on" method="POST">
    <button class="on" type="submit">LED ON</button>
  </form>
  <form action="/off" method="POST">
    <button class="off" type="submit">LED OFF</button>
  </form>
)HTML";

  // ★ BUZZER: display its current state
  page += "<hr><h2>Buzzer is ";
  page += buzzerOn ? "ON" : "OFF";
  page += "</h2>";

  // ★ BUZZER: webpage buttons
  page += R"HTML(
  <form action="/buzzer/on" method="POST">
    <button class="on" type="submit">Buzzer ON</button>
  </form>
  <form action="/buzzer/off" method="POST">
    <button class="off" type="submit">Buzzer OFF</button>
  </form>
)HTML";
  // ★ BUZZER: end of webpage additions

  page += "</body></html>";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", page);
}

void setLed(bool state) {
  ledOn = state;
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

// ★ BUZZER: switch the buzzer and return to the webpage
void setBuzzer(bool state) {
  buzzerOn = state;
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);

  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}
// ★ BUZZER: end of control function

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ★ BUZZER: configure output and start switched off
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

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

  server.on("/on", HTTP_POST, []() {
    setLed(true);
  });

  server.on("/off", HTTP_POST, []() {
    setLed(false);
  });

  // ★ BUZZER: handle requests from the buzzer buttons
  server.on("/buzzer/on", HTTP_POST, []() {
    setBuzzer(true);
  });

  server.on("/buzzer/off", HTTP_POST, []() {
    setBuzzer(false);
  });
  // ★ BUZZER: end of routes

  server.begin();
}

void loop() {
  server.handleClient();
  delay(2);
}
