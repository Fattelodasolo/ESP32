#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define LED 2           // Il led è collegato al pin D2

const char *ssid = "ESP32-WiFi";
const char *password = "fattelodasolo";

WiFiServer server(80);        // porta protocollo HTTP


void setup() {
  pinMode(LED, OUTPUT);       // direzione del pin 2

  Serial.begin(9600);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();     // ottengo IP
  Serial.print("AP IP address: ");
  Serial.println(myIP);                // stampo su seriale l'IP dell'access point             
  server.begin();                     // avvio server
}

void loop() {
  WiFiClient client = server.available();   // ascolta le richieste di nuovi client

  if (client) {                             // se c'è un nuovo client,
    Serial.println("Nuovo Client!");        // stampa un messaggio su seriale
    String currentLine = "";                // crea una stringa per contenere le informazioni dal client
    while (client.connected()) {            // mentre il client e connesso...
      if (client.available()) {             // se ci sono byte dal client,
        char c = client.read();             // li legge
        Serial.write(c);                    // e li stampa su monitor seriale
        if (c == '\n') {                    // se il byte è una nuova linea

          /* e se la linea successiva è vacante, vengono letti entrambi come due caratteri di nuova linea.
          Viene interpretata come la fine della richiesta del Client in HTTP e si ottiene la risposta:*/
          if (currentLine.length() == 0) {
            /*Le intestazioni HTTP iniziano sempre con un codice di risposta (es: HTTP/1.1 200 OK) 
            e un tipo di contenuto, in modo che il client sappia cosa sta arrivando, quindi una riga vuota:*/
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // il contenuto della risposta
            client.print("Clicca <a href=\"/H\">qui</a> per accendere il LED.<br>");
            client.print("Clicca <a href=\"/L\">qui</a> per spegnere il LED.<br>");

            // la risposta HTTP ci conclude con una nuova linea
            client.println();
            // si interrompe il ciclo While
            break;
          } else {    // in caso di nuova linea, si pulisce la stringa currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // se riceviamo il carattere di fine linea /r
          currentLine += c;      // aggiungilo alla fine della stringa currentLine
        }

        // viene controllato se la risposta del Client è "GET /H" o "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED, HIGH);               // GET /H LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED, LOW);                // GET /L LED off
        }
      }
    }
    // chiude la connessione con il client
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
