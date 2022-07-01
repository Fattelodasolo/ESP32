#include <WiFi.h>
#include <ESPmDNS.h>

const char* ssid = "nome-rete";
const char* password = "password-rete";

IPAddress local_IP(192, 168, 1, 153);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

String header;
String stato1 = "off";
String stato2 = "off";
String stato3 = "off";
String stato4 = "off";
String stato5 = "off";

void setup() {
  pinMode(12, OUTPUT);          // relè R2
  pinMode(13, OUTPUT);          // relè R1
  pinMode(18, OUTPUT);          // relè R4
  pinMode(19, OUTPUT);          // relè R5
  pinMode(25, OUTPUT);          // relè R3
  pinMode(4, INPUT);            // pulsante S3
  pinMode(5, INPUT);            // pulsante S4
  pinMode(23, INPUT);           // pulsante S5
  pinMode(27, INPUT);           // pulsante S6
  pinMode(26, INPUT);           // pulsante S7

  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(25, HIGH);

  Serial.begin(9600);

  if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("Impossibile assegnare IP Statico");
  }
  
  WiFi.begin(ssid, password);
  Serial.print("Connessione");
  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  Serial.println("Connessione WiFi avvenuta!");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP() );
    
  if (MDNS.begin("multisocket")) {
    Serial.println("http://multisocket.local/");
  }

  server.begin();
  
}

void loop() {
  while (digitalRead(4)==HIGH){
    digitalWrite(13, !digitalRead(13));
    delay (500);
  }

    while (digitalRead(5)==HIGH){
    digitalWrite(12, !digitalRead(12));
    delay (500);
  }

    while (digitalRead(23)==HIGH){
    digitalWrite(25, !digitalRead(25));
    delay (500);
  }

    while (digitalRead(27)==HIGH){
    digitalWrite(18, !digitalRead(18));
    delay (500);
  }

    while (digitalRead(26)==HIGH){
    digitalWrite(19, !digitalRead(19));
    delay (500);
  }

  // Verifica che sia connesso un client
  WiFiClient client = server.available();
 if (client) {                             // Se un Client si connette...
   currentTime = millis();
   previousTime = currentTime;
    Serial.println("Nuovo Client");          // stampa un messaggio su seriale
    String currentLine = "";                // realizza una stringa per conservare i dati in arrivo dal client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // mentre il client rimane connesso...
      currentTime = millis();
      if (client.available()) {             // se ci sono dati dal client li legge,
        char c = client.read();
        Serial.write(c);                    // li stampa su monitor seriale
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {    // Se la linea corrente è vuota, si attende la risposta
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Cambia lo stato da on a off dei PIN
            if (header.indexOf("GET /Relay01/on") >= 0) {
              Serial.println("Attiva relè 1");
              stato1 = "on";
              digitalWrite(13, LOW);
            }
            else if (header.indexOf("GET /Relay01/off") >= 0) {
              Serial.println("Spegni relè 1");
              stato1 = "off";
              digitalWrite(13, HIGH);
            }
            else if (header.indexOf("GET /Relay02/on") >= 0) {
              Serial.println("Attiva relè 2");
              stato2 = "on";
              digitalWrite(12, LOW);
            }
            else if (header.indexOf("GET /Relay02/off") >= 0) {
              Serial.println("Spegni relè 2");
              stato2 = "off";
              digitalWrite(12, HIGH);
            }
            else if (header.indexOf("GET /Relay03/on") >= 0) {
              Serial.println("Attiva relè 3");
              stato3 = "on";
              digitalWrite(25, LOW);
            }
            else if (header.indexOf("GET /Relay03/off") >= 0) {
              Serial.println("Spegni relè 3 ");
              stato3 = "off";
              digitalWrite(25, HIGH);
            }
            else if (header.indexOf("GET /Relay04/on") >= 0) {
              Serial.println("Attiva relè 4");
              stato4 = "on";
              digitalWrite(18, LOW);
            }
            else if (header.indexOf("GET /Relay04/off") >= 0) {
              Serial.println("Spegni relè 4 ");
              stato4 = "off";
              digitalWrite(18, HIGH);
            }
            else if (header.indexOf("GET /Relay05/on") >= 0) {
              Serial.println("Attiva relè 5");
              stato5 = "on";
              digitalWrite(19, LOW);
            } 
            else if (header.indexOf("GET /Relay05/off") >= 0) {
              Serial.println("Spegni relè 5 ");
              stato5 = "off";
              digitalWrite(19, HIGH);
            }
            
            // Mostra all'indirizzo creato i bottoni personalizzati
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: groove; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #F14E4E;}</style></head>");
            //Mostra titolo
            client.println("<body style='background-color:darkslategray'><h1><font color='white'>Multipresa WiFi</font></h1>");
            
            client.println("<p><font color='white'>Rele' 01</font></p>");
            if (stato1=="off") {
              client.println("<p><a href=\"/Relay01/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Relay01/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("<p><font color='white'>Rele' 02</font></p>");
            if (stato2=="off") {
              client.println("<p><a href=\"/Relay02/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Relay02/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("<p><font color='white'>Rele' 03</font></p>");
            if (stato3=="off") {
              client.println("<p><a href=\"/Relay03/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Relay03/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("<p><font color='white'>Rele' 04</font></p>");
            if (stato4=="off") {
              client.println("<p><a href=\"/Relay04/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Relay04/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("<p><font color='white'>Rele' 05</font></p>");
            if (stato5=="off") {
              client.println("<p><a href=\"/Relay05/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Relay05/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            client.println();
            
            // Interrompiamo a questo punto il ciclo while
            break;
          } else { // se non c'è altro, ripuliamo la stringa
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    header = "";
    
    client.stop();
    Serial.println("Client disconnesso.");
    Serial.println("");
 }
}
