#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

// ====== WiFi ======
const char* WIFI_SSID = "YourWiFi";
const char* WIFI_PASS = "YourWiFiPassword";

// ====== Google Apps Script Web App URL ======
const char* SCRIPT_URL = "YOUR URL";

// Must match Apps Script SHARED_SECRET
const char* SHARED_SECRET = "CHANGE_ME_TO_A_RANDOM_STRING";

// ====== DHT11 on GPIO32 ======
#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ====== Send interval ======
unsigned long lastSendMs = 0;
const unsigned long SEND_EVERY_MS = 10000; // 5 seconds

WiFiClientSecure client;

// ---- URL helpers ----
static String getHostFromUrl(const String& url) {
  int hostStart = url.indexOf("://");
  hostStart = (hostStart >= 0) ? hostStart + 3 : 0;
  int hostEnd = url.indexOf('/', hostStart);
  if (hostEnd < 0) hostEnd = url.length();
  return url.substring(hostStart, hostEnd);
}

static String getPathFromUrl(const String& url) {
  int hostStart = url.indexOf("://");
  hostStart = (hostStart >= 0) ? hostStart + 3 : 0;
  int pathStart = url.indexOf('/', hostStart);
  if (pathStart < 0) return "/";
  return url.substring(pathStart);
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("✅ Connected. IP: ");
  Serial.println(WiFi.localIP());
}

bool postJsonToScript(float tempC, float humPct) {
  const String url = String(SCRIPT_URL);
  const String host = getHostFromUrl(url);
  const String path = getPathFromUrl(url);

  const int httpsPort = 443;

  // Demo-mode TLS: skips certificate verification (easy + works reliably)
  client.setInsecure();

  if (!client.connect(host.c_str(), httpsPort)) {
    Serial.println("❌ HTTPS connection failed");
    return false;
  }

  // JSON payload (Apps Script expects these keys)
  String payload = "{";
  payload += "\"secret\":\"" + String(SHARED_SECRET) + "\",";
  payload += "\"temperature\":" + String(tempC, 1) + ",";
  payload += "\"humidity\":" + String(humPct, 1) + ",";
  payload += "\"device\":\"ESP32-DHT11\"";
  payload += "}";

  // HTTP request
  client.println("POST " + path + " HTTP/1.1");
  client.println("Host: " + host);
  client.println("User-Agent: ESP32");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(payload.length());
  client.println("Connection: close");
  client.println();
  client.print(payload);

  // Read status line
  String statusLine = client.readStringUntil('\n');
  statusLine.trim();
  Serial.println("➡️ " + statusLine);

  // Drain response (optional)
  while (client.connected() || client.available()) {
    if (client.available()) {
      client.readStringUntil('\n');
    }
  }
  client.stop();

  return statusLine.indexOf("200") >= 0;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  dht.begin();
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  unsigned long now = millis();
  if (now - lastSendMs >= SEND_EVERY_MS) {
    lastSendMs = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Celsius

    if (isnan(h) || isnan(t)) {
      Serial.println("❌ Failed to read from DHT11");
      return;
    }

    Serial.printf("Temp: %.1f C | Hum: %.1f %%\n", t, h);

    bool ok = postJsonToScript(t, h);
    Serial.println(ok ? "✅ Logged to Google Sheet" : "❌ Logging failed");
  }
}
