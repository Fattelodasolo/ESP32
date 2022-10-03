#include "BluetoothSerial.h"    // Libreria per Bluetooth

BluetoothSerial BT;             // Oggetto per comunicare

void setup() {
  Serial.begin(9600);
  BT.begin("ESP32");            //Nome dispositivo Bluetooth
  
}

void loop() {
 while (Serial.available()) {      // Mentre la comunicazione seriale è avviata...
    BT.write(Serial.read());        // il dispositivo scrive ciò che legge
    }
    
 while (BT.available()) {          // Mentre la comunicazione Bluetooth è avviata...
    Serial.write(BT.read());
    }
  
}
