// =============================================================================
// Ko.h - request queue
//
// Stores the requests for each floor, reads the floor buttons, the joystick
// and the rotary encoder, drives the floor LEDs and handles the emergency
// mode. Also finds the next stop in each direction.
// =============================================================================

#ifndef KO_H
#define KO_H

#include <Arduino.h>
#include "Posisjon.h"

class Ko {
  public:
    Ko();
    void begin();
    void oppdater();                                   // read all inputs, called every 20 ms

    // Queries and changes to the queue
    bool erBestilt(int etasje);                        // any request at this floor?
    bool skalBetjenes(int etasje, int retning);        // request to serve in this direction?
    void fjernBestilling(int etasje, int retning);
    int  nesteStoppOver(int etasje);
    int  nesteStoppUnder(int etasje);
    void leggTilUtvendig(int etasje, int retning);     // add a hall call

    // State of the hall call menu and the emergency mode
    int  valgtEtasje();
    int  valgtRetning();
    bool visMeny();
    int  count();
    bool EMERGENCY();

  private:
    // Requests, one entry per floor
    bool _forrigeKnapp[ANTALL_ETASJER];        // last state of each floor button
    bool _bestiltInne[ANTALL_ETASJER];         // cabin calls
    bool _bestiltOpp[ANTALL_ETASJER];          // hall calls up
    bool _bestiltNed[ANTALL_ETASJER];          // hall calls down
    bool _bestiltEMERGENCY[ANTALL_ETASJER];    // emergency stops

    // Hall call menu (joystick)
    int  _valgtEtasje;
    int  _valgtRetning;

    // Emergency mode (rotary encoder)
    int  _EMERGENCYcount;                      // encoder steps turned
    int  _resetEMERGENCY;                      // button presses towards reset
    bool _setEMERGENCY;                        // emergency mode active

    // Edge detection for joystick, button and encoder, and the menu flag
    bool _joyXVarMidt;
    bool _forrigeEncKnapp;
    bool _visMeny;
    bool _naaEncA;
    bool _naaEncB;
    bool _forrigeEncA;
    bool _forrigeEncB;
};
#endif
