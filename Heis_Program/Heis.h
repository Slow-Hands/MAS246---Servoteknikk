// =============================================================================
// Heis.h - state machine of the elevator
//
// Decides what the elevator does: stand still, drive up or down, or open and
// close the door. Owns the request queue (Ko) and chooses the target floor.
// =============================================================================

#ifndef HEIS_H
#define HEIS_H

#include <Arduino.h>
#include "Ko.h"
#include "door.h"

class Heis {
  public:
    // The six states of the state machine
    enum Tilstand { STAAR, KJORER_OPP, KJORER_NED, DOR_AAPEN, DOR_AAPNER, DOR_LUKKER };

    Heis();
    void begin();
    void oppdater(int etasjeNaa, long pos, Door& dor, bool overvekt);   // run one step of the state machine
    int maalEtasje();
    Tilstand tilstand();

    // Pass-through to the queue, used by the LCD and the serial output
    int valgtEtasje();
    int valgtRetning();
    bool visMeny();
    int count();
    bool EMERGENCY();



  private:
    void byttTil(Tilstand ny);   // change state and restart the state timer

    Ko _ko;                      // request queue
    Tilstand _tilstand;          // current state
    int _maalEtasje;             // target floor
    unsigned long _tidInn;       // time when the current state was entered [ms]
    int _retning;                // last travel direction: 1 = up, -1 = down
    int _videre;                 // direction the elevator continues in after the stop
};

#endif
