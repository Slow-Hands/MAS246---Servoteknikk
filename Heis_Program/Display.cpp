// =============================================================================
// Display.cpp - 16x2 LCD
// =============================================================================

#include "Display.h"

static const int LCD_BACKLIGHT = 4;   // backlight pin

// ---------------------------------------------------------------------------
// Custom characters (5x8 pixels) for the up and down arrows
// ---------------------------------------------------------------------------
static byte pilOpp[8] = {
  B00100, B01110, B11111, B00100,
  B00100, B00100, B00100, B00000
};

static byte pilNed[8] = {
  B00000, B00100, B00100, B00100,
  B00100, B11111, B01110, B00100
};

// ---------------------------------------------------------------------------
// Constructor: LCD in 4-bit mode on RS 41, E 40, D4-D7 37-34.
// The stored values are set so the first call always redraws.
// ---------------------------------------------------------------------------
Display::Display()
  : _lcd(41, 40, 37, 36, 35, 34) {
  _forrigeEtasje = -1;
  _forrigePil = 2;
  _forrigeVarMeny = false;
}

// ---------------------------------------------------------------------------
// Turn on the backlight, start the LCD and load the arrow characters
// ---------------------------------------------------------------------------
void Display::begin() {
  pinMode(LCD_BACKLIGHT, OUTPUT);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  _lcd.begin(16, 2);
  _lcd.createChar(0, pilOpp);
  _lcd.createChar(1, pilNed);
  _lcd.clear();
}

// ---------------------------------------------------------------------------
// Emergency mode: only show "EMERGENCY!". The stored floor is reset so the
// normal view is redrawn when the emergency mode ends.
// ---------------------------------------------------------------------------
void Display::EMERGENCY() {
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.print("EMERGENCY!");
  _forrigeEtasje = -1;
}

// ---------------------------------------------------------------------------
// Normal view, line 1: "Etg. k" and the direction arrow
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Hall call menu: selected floor and direction on line 2
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Line 2: the load in kg, or OVERVEKT when the cabin is overloaded.
// Trailing spaces overwrite old characters without clearing the screen.
// ---------------------------------------------------------------------------
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
