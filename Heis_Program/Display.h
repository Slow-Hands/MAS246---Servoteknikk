// =============================================================================
// Display.h - 16x2 LCD
//
// Shows the floor and direction arrow, the load, the hall call menu and the
// emergency text. Only redraws when the content changes, to avoid flicker.
// =============================================================================

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class Display {
  public:
    Display();
    void begin();
    void vis(int etasje, int pil);            // normal view: floor and arrow
    void visMeny(int etasje, int pil);        // hall call menu
    void EMERGENCY();                         // emergency text
    void visVekt(int verdi, bool overvekt);   // load or OVERVEKT on line 2

  private:
    LiquidCrystal _lcd;
    // What is on the screen now, so it is only redrawn when something changes
    int _forrigeEtasje;
    int _forrigePil;
    bool _forrigeVarMeny;
};

#endif
