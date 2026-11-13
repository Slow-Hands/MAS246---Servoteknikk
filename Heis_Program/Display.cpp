#include "Display.h"

static const int LCD_BACKLIGHT = 4;

static byte pilOpp[8] = {
  B00100, B01110, B11111, B00100,
  B00100, B00100, B00100, B00000
};

static byte pilNed[8] = {
  B00000, B00100, B00100, B00100,
  B00100, B11111, B01110, B00100
};

Display::Display()
  : _lcd(41, 40, 37, 36, 35, 34) {
  _forrigeEtasje = -1;
  _forrigePil = 2;
  _forrigeVarMeny = false;
}

void Display::begin() {                                 
  pinMode(LCD_BACKLIGHT, OUTPUT);                       
  digitalWrite(LCD_BACKLIGHT, HIGH);                    
  _lcd.begin(16, 2);                                    
  _lcd.createChar(0, pilOpp);                           
  _lcd.createChar(1, pilNed);                           
  _lcd.clear();                                         
}  
                                                     
void Display::EMERGENCY() {  
  _lcd.clear();                                       
  _lcd.setCursor(0, 0);                               
  _lcd.print("EMERGENCY!");
  _forrigeEtasje = -1; 
}

void Display::vis(int etasje, int pil) {
  if (etasje != _forrigeEtasje || pil != _forrigePil || _forrigeVarMeny) {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Etg. ");
    _lcd.print(etasje);

    _lcd.setCursor(6, 0);
    if (pil == 1) {
      _lcd.write(byte(0));
    } else if (pil == -1) {
      _lcd.write(byte(1));
    }

    _forrigeEtasje = etasje;
    _forrigePil = pil;
    _forrigeVarMeny = false;
  }
}

void Display::visMeny(int etasje, int pil) {
  if (etasje != _forrigeEtasje || pil != _forrigePil || !_forrigeVarMeny) {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Bestill etasje:");

    _lcd.setCursor(0, 1);
    _lcd.print("Etg. ");
    _lcd.print(etasje);

    _lcd.setCursor(9, 1);
    if (pil == 1) {
      _lcd.write(byte(0));
    } else if (pil == -1) {
      _lcd.write(byte(1));
    }



    _forrigeEtasje = etasje;
    _forrigePil = pil;
  }
  _forrigeVarMeny = true;
}

void Display::visVekt(int verdi, bool overvekt) {
  _lcd.setCursor(0, 1);
  if (overvekt) {
    _lcd.print("OVERVEKT! ");
  } else {
    _lcd.print("Last:     ");
  }
  _lcd.print(verdi);
  _lcd.print("kg  ");
}