#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Wi-Fi settings
const char* ssid = "156HomeKitchen";
const char* password = "Hitchin786&";

// DHT11 sensor
constexpr int DHT_PIN = 32;
DHT dht(DHT_PIN, DHT11);

// Web server
WebServer server(80);

void handleRoot() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  bool sensorWorking =
      !isnan(temperature) && !isnan(humidity);

  String page = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="refresh" content="10">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 DHT Server</title>

  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f4f4f4;
      color: #444;
      margin: 0;
      padding: 20px;
    }

    .container {
      max-width: 600px;
      margin: 40px auto;
      padding: 24px;
      text-align: center;
      background: white;
      border-radius: 12px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }

    h2 {
      color: #c62828;
    }

    .reading {
      font-size: 2rem;
    }

    .success {
      color: #16803c;
    }

    .error {
      color: #c62828;
      background: #ffebee;
      padding: 20px;
      border-radius: 8px;
      font-size: 1.3rem;
    }

    .footer {
      margin-top: 30px;
      font-size: 0.9rem;
      color: #777;
    }
  </style>
</head>

<body>
  <div class="container">
    <h2>ESP32 DHT Sensor Data</h2>
)HTML";

  if (sensorWorking) {
    page += "<p class='success'>Sensor readings received</p>";

    page += "<p class='reading'>Temperature: <strong>";
    page += String(temperature, 1);
    page += "&deg;C</strong></p>";

    page += "<p class='reading'>Humidity: <strong>";
    page += String(humidity, 1);
    page += "%</strong></p>";

    Serial.printf(
        "Temperature: %.1f C | Humidity: %.1f %%\n",
        temperature, humidity
    );
  } else {
    page += "<p class='error'>"
            "DHT sensor is not working.<br>"
            "Check the sensor connection."
            "</p>";

    Serial.println("Failed to read from DHT sensor!");
  }

  page += R"HTML(
    <p class="footer">
      Data refreshed every 10 seconds
    </p>
  </div>
</body>
</html>
)HTML";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", page);
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("Open http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2);
}
