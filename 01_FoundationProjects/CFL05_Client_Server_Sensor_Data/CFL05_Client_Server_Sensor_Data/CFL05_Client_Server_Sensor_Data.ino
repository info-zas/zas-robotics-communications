#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char *ssid = "Enter Your WiFi SSID";
const char *password = "Enter Your Password";

WebServer server(80);
DHT dht(33, DHT11);  // DHT11 on GPIO 33

void handleRoot() {
  char msg[2000];

  snprintf(msg, 2000,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='10'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link href='https://fonts.googleapis.com/css2?family=Roboto:wght@100;400&display=swap' rel='stylesheet'>\
    <title>ESP32 DHT Server</title>\
    <style>\
      body { font-family: 'Roboto', sans-serif; background-color: #f4f4f4; color: #444; margin: 0; padding: 0; }\
      .container { max-width: 600px; margin: 50px auto; padding: 20px; text-align: center; background-color: white; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }\
      h2 { font-weight: 400; }\
      .reading { font-size: 2.5rem; margin: 20px 0; }\
      .units { font-size: 1.5rem; color: #888; }\
      .sensor-icon { font-size: 4rem; color: #1a8cff; margin-bottom: 10px; }\
      .footer { margin-top: 30px; font-size: 0.8rem; color: #aaa; }\
    </style>\
  </head>\
  <body>\
    <div class='container'>\
      <h2 style='color:red'>ESP32 DHT Sensor Data</h2>\
      <p class='sensor-icon'><i class='fas fa-thermometer-half'></i></p>\
      <p class='reading'>Temperature: <strong>%.2f&deg;C</strong></p>\
      <p class='sensor-icon'><i class='fas fa-tint'></i></p>\
      <p class='reading'>Humidity: <strong>%.2f%%</strong></p>\
      <p class='footer'>Data refreshed every 10 seconds</p>\
    </div>\
  </body>\
</html>",
           readDHTTemperature(), readDHTHumidity());

  server.send(200, "text/html", msg);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2000); // Allow CPU time for other tasks
}

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.print("Temp: ");
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.print("Humi: ");
    Serial.println(h);
    return h;
  }
}
