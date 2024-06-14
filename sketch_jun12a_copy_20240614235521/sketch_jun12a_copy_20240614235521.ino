#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 2 
#define DHTTYPE DHT11 

DHT dht(2, DHT11);

const char* ssid = "ZTE-d23580";
const char* password = "Rama2021";
const char* serverName = "Https:94.140.14.14/Coba"; 

unsigned long timerDelay = 3000;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 3 seconds (timerDelay variable), it will take 3 seconds before publishing the first reading.");
  dht.begin();
}

void loop() {
  if (millis() - lastTime > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);

      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }

      float hif = dht.computeHeatIndex(f, h);
      float hic = dht.computeHeatIndex(t, h, false);

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print(f);
      Serial.print(F("°F  Heat index: "));
      Serial.print(hic);
      Serial.print(F("°C "));
      Serial.print(hif);
      Serial.println(F("°F"));

      // Debugging: Print the JSON data to be sent
      String jsonData = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}";
      Serial.print("JSON data to send: ");
      Serial.println(jsonData);

      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String payload = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println(payload); 
      } else {
        Serial.println("Error on HTTP request: " + String(httpResponseCode));
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}