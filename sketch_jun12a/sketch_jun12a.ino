#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h> // Jika menggunakan sensor DHT untuk contoh

#define DHTPIN 4 // Pin data DHT
#define DHTTYPE DHT11 // Tipe DHT (DHT11, DHT22, dll.)

const char* ssid = "Nama_WiFi_Anda";
const char* password = "Password_WiFi_Anda";
const char* serverName = "http://IP_Local_Server_Anda/endpoint"; // Ganti dengan alamat server Anda

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Menghubungkan ke WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Terhubung!");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor!");
    return;
  }

  if (client.connect(serverName, 80)) { // Port 80 untuk HTTP
    String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
    
    client.println("POST /endpoint HTTP/1.1");
    client.println("Host: IP_Local_Server_Anda"); // Ganti dengan IP server Anda
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postData.length()));
    client.println();
    client.println(postData);

    Serial.println("Data terkirim!");
  } else {
    Serial.println("Gagal terhubung ke server!");
  }

  delay(2000); // Kirim data setiap 2 detik
}
