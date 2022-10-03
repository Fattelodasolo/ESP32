#include "SPI.h"
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "SPIFFS.h"         // Libreria per caricare i files

#include <TJpg_Decoder.h>   // decodifica dei file jpg

int Img;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n Testing TJpg_Decoder library");

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS inizializzazione finita!");
    while (1) yield(); // Attendi
  }
  Serial.println("\r\nInizializzazione conclusa.");

  listSPIFFS();

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // L'immagine jpeg può essere ridimensionata di un fattore di 1, 2, 4 o 8
  TJpgDec.setJpgScale(1);

  // L'ordine dei byte può essere scambiato (imposta true per TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // Al decoder deve essere assegnato il nome esatto della funzione di rendering
  TJpgDec.setCallback(tft_output);
}

void loop() {
  if (Img > 27){        // la variabile raggiunto il massimo si azzera
    Img = 0;
  }

  String imgPath = "/frame_";           //I file iniziano con questa dicitura...
  imgPath += Img++;                     // incremento il numero...
  imgPath += ".jpg";                    // pongo l'estensione al file

  TJpgDec.drawFsJpg(0, 0, imgPath);     // stampo sul display i file così letti
}

void listSPIFFS(void) {
  Serial.println(F("\r\nLista SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Impossibile aprire la directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Non è una directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // Il file path può essere di 31 ccaratteri al massimo in SPIFFS
      int spaces = 33 - fileName.length(); // tabulare bene
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // tabulare bene
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Interrompi l'ulteriore decodifica poiché l'immagine scorre dalla parte inferiore dello schermo
  if ( y >= tft.height() ) return 0;

  // Questa funzione ritaglierà automaticamente il rendering del blocco di immagine ai limiti del TFT
  tft.pushImage(x, y, w, h, bitmap);

  // Ritorna 1 per decodificare il blocco successivo
  return 1;
}
