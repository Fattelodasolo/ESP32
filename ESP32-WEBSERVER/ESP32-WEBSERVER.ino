#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* SSID = "nomerete";
const char* PASSWORD = "passwordrete";

WebServer server(80);

String stringa = "Hello World!";

void setup() {
Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connettiti a  ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.print("ESP32 online: ");
    Serial.println("http://esp32.local/");
  }

  server.on("/", handleRoot);
  server.begin();

}

void loop() {
  server.handleClient();

}

void handleRoot() {
  //server.send(200, "text/plain", "Hello World");
  server.send(200, "text/html","<h1>ESP32:" + stringa + "</h1>");
}
