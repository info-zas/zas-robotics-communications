#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ===== DHT CONFIG =====
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== ACCESS POINT CONFIG =====
const char* ap_ssid = "Agri_Field_Node";
const char* ap_password = "farm12345";  // Min 8 characters

WebServer server(80);

// ===== SENSOR DATA =====
float temperature = 0.0;
float humidity = 0.0;

// ===== HTML DASHBOARD =====
void handleRoot() {
  char msg[1200];

  snprintf(msg, sizeof(msg),
           "<html><head>"
           "<meta name='viewport' content='width=device-width, initial-scale=1'>"
           "<meta http-equiv='refresh' content='5'/>"
           "<title>Agriculture Field Monitor</title>"
           "<style>"
           "body{font-family:Arial;background:#f4f4f4;text-align:center;}"
           ".card{background:white;padding:20px;margin:30px;border-radius:10px;box-shadow:0 0 10px #aaa;}"
           "</style>"
           "</head><body>"
           "<div class='card'>"
           "<h2 style='color:green'>🌾 Agriculture Field Sensor</h2>"
           "<p style='font-size:1.8rem'>Temperature: <b>%.2f °C</b></p>"
           "<p style='font-size:1.8rem'>Humidity: <b>%.2f %%</b></p>"
           "<p style='color:gray'>Offline | Direct ESP32 Access</p>"
           "</div>"
           "</body></html>",
           temperature, humidity);

  server.send(200, "text/html", msg);
}

void setup() {
  Serial.begin(115200);

  // ===== START SENSOR =====
  dht.begin();

  // ===== START ACCESS POINT =====
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("ESP32 Agriculture Node Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());  // Usually 192.168.4.1

  // ===== HTTP SERVER =====
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server running");
}

void loop() {
  server.handleClient();

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 3000) {
    lastRead = millis();

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read DHT sensor");
    } else {
      Serial.print("Temp: ");
      Serial.print(temperature);
      Serial.print(" °C | Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    }
  }
}
