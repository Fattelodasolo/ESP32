#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 20, 4); 


void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Hello World! ");
  
  lcd.setCursor(0,1);
  lcd.print("Test LCD I2C");

  lcd.setCursor(0,2);
  lcd.print("by");

  lcd.setCursor(0,3);
  lcd.print("Reshitoze");

  delay(5000);

  lcd.clear();
}

void loop() {

  lcd.setCursor(5,1);
  lcd.print("ISCRIVITI");

  lcd.setCursor(5,2);
  lcd.print("AL CANALE");

  delay(1000);

  lcd.clear();

  delay(1000);
  

}
