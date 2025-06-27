//LIBRARIES

#include <WiFi.h>
#include "DHT.h"

//DEFINITIONS

#define DHTPIN 4                      //DHT DATA TO PIN 4
#define DHTTYPE DHT11                 //SENSOR MODEL(WEATHER)
#define LED_TEMP 5                    //LED FOR DTH
#define PIR_PIN 14                    //PIR OUT TO PIN 14
#define LED_MOTION 12                 //LED FOR PIR

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "ESPTest";         // Your hotspot SSID
const char* password = "12345678";    // Your hotspot password

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_TEMP, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_MOTION, OUTPUT);
  digitalWrite(LED_TEMP, LOW);
  digitalWrite(LED_MOTION, LOW);

  Serial.println("📶 Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {         //ESP32 STARTS CONNECTING TO HOTSPOT
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected!");             //KEEP WAITING UNTIL THE WiFi CONNECTS
  Serial.print("🌐 IP Address: ");
  Serial.println(WiFi.localIP());                //PRINTS IP ADDRESS

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int motion = digitalRead(PIR_PIN);

  // LED alerts
  digitalWrite(LED_TEMP, temp > 40 ? HIGH : LOW);
  digitalWrite(LED_MOTION, motion == HIGH ? HIGH : LOW);

  if (client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML><html><head><meta http-equiv='refresh' content='3'></head>");     //AUTO REFRESHES PAGE EVERY 3 SEC
    client.println("<h2>Smart Weather + Motion Monitoring</h2>");
    client.print("Temperature: ");
    client.print(temp);
    client.println(" &deg;C<br>");
    client.print("Humidity: ");
    client.print(hum);
    client.println(" %<br>");
    client.print("Motion Detected: ");
    client.println(motion == HIGH ? "Yes" : "No");
    client.println("</html>");

    delay(1000);
    client.stop();
  }
}
