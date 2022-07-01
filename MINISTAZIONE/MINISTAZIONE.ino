#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "weathericons.h"

WiFiClient client;

TFT_eSPI tft = TFT_eSPI(); 
 
const char* ssid = "wifi";
const char* password =  "password";

const String city_id = "xxxxxxx"; 
const String apikey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const String units = "metric";

char url[] = "api.openweathermap.org";

String info = "";

void setup() {
 
  Serial.begin(9600);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.drawBitmap(70, 0, logo, 100, 100,TFT_RED);
  delay(1500);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Connessione a rete WiFi...", 0, 110);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
 
  Serial.println("Connesso");
  Serial.println("");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

 
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Connesso!", 160, 110);

  delay(1000);

  tft.fillScreen(TFT_BLACK);
 
}
 
void loop() {
 
  if (client.connect(url, 80))
  { //avvia la connessione del client
    client.println("GET /data/2.5/weather?id=" + city_id + "&units=" + units + "&APPID=" + apikey);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");        //connessione fallita
    Serial.println();
  }

   while (client.connected() && !client.available())          // attendo mentre il client si è collegato ed è disponibile
    delay(1);
  while (client.connected() || client.available())            // mentre il client è connesso e disponibile, leggo i dati
  { 
    char reading = client.read();
    info = info + reading;
  }

  client.stop();                                              // interrompo la connessione del client

  // Elaboro i dati ricevuti in una sola volta e li smembro, in modo da ottenere le singole informazioni utili
  info.replace('[', ' ');
  info.replace(']', ' ');
  char jsonArray [info.length() + 1];
  info.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[info.length() + 1] = '\0';
  StaticJsonDocument<1024> doc;
  DeserializationError  error = deserializeJson(doc, jsonArray);


  if (error) {
    Serial.print(F("deserializeJson() fallito "));
    Serial.println(error.c_str());
    return;
  }

  String location = doc["name"];            // Città
  String country = doc["sys"]["country"];   // Paese
  int t = doc["main"]["temp"];              // Temperatura
  int h = doc["main"]["humidity"];          // Umidità
  int p = doc["main"]["pressure"];          // Pressione
  String wind = doc["wind"]["speed"];       // Velocità del vento
  String weather = doc["weather"]["main"];  // Condizione meteo
  String icon = doc["weather"]["icon"];     // ID icona

  Serial.println();
  Serial.print("Paese: ");
  Serial.println(country);
  Serial.print("Luogo: ");
  Serial.println(location);
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.printf("Umidità: ");
  Serial.print(h);
  Serial.println("%");
  Serial.printf("Pressione: ");
  Serial.print(p);
  Serial.println(" mBar");
  Serial.printf("Vento: ");
  Serial.print(wind);
  Serial.println(" m/s");
  Serial.print("Meteo: ");
  Serial.print(weather);
  Serial.print(" ");
  Serial.println (icon);

// Stampo IP, città, Paese
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString(WiFi.localIP().toString().c_str(), 3, 10);
  tft.drawString(country, 220, 10);
  tft.drawString(location, 175, 10);
  tft.fillRect (0, 25, 320, 2, TFT_YELLOW);
// Stampo Temperatura e umidità
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawNumber(t, 60, 45);
  tft.drawString("C - ", 90, 45);
  tft.drawNumber(h, 135, 45);
  tft.drawString("%", 162, 45);
// Stampo pressione in millibar
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawNumber(p, 60, 65);
  tft.drawString("mBar", 115, 65);
// Stampo velocità del vento e relativa icona
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(wind, 60, 103);
  tft.drawString("m/s", 120, 103);
  tft.drawBitmap(0, 93, windicon, 50, 38,TFT_GREEN);

// In base al codice meteo, applico un'icona
  if (icon == "01d" || icon == "01n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, sunny , 43, 43,TFT_WHITE);          // soleggiato
          
  }
  else if (icon == "02d" || icon == "02n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, partlysunny ,52, 36,TFT_WHITE);     // parzialmente soleggiato
          
  }
  else if (icon == "03d" || icon == "03n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, partlycloudy ,52, 34,TFT_WHITE);    // parzialmente nuvoloso
    
  }
  else if (icon == "04d" || icon == "04n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, cloudy ,47, 29,TFT_WHITE);          // nuvoloso
      
    
  }
  else if (icon == "09d" || icon == "09n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, rain ,46, 42,TFT_WHITE);            // piovoso
    
  }
  else if (icon == "10d" || icon == "10n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, rain ,46, 42,TFT_WHITE);            // piovoso
    
  }
  else if (icon == "11d" || icon == "11n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, tstorms ,46, 44,TFT_WHITE);         // temporale
    
  }
  else if (icon == "13d" || icon == "13n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, snow ,49, 39,TFT_WHITE);            // nevoso
    
  }
  else if (icon == "50d" || icon == "50n"){
      tft.fillRect (0, 45, 52, 52, TFT_BLACK);
      tft.drawBitmap(0, 45, fog , 35, 32, TFT_WHITE);           // nebbioso
    
  }
  else {
    tft.fillRect (0, 45, 52, 52, TFT_BLACK);
    tft.drawBitmap(0, 45, unknown, 32, 32, TFT_WHITE);          // sconosciuto
    }


    info = "";

    delay(5000);

}
