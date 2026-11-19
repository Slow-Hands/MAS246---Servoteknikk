// =============================================================================
// Heis.cpp - state machine of the elevator
//
// STAAR       standing still at a floor, door closed
// KJORER_OPP  moving up towards the target floor
// KJORER_NED  moving down towards the target floor
// DOR_AAPNER  door is opening
// DOR_AAPEN   door is open, waits DOR_TID before closing
// DOR_LUKKER  door is closing
//
// The cabin only leaves a floor from STAAR or DOR_LUKKER with the door
// closed, so the door is always closed while the cabin moves.
// =============================================================================

#include "Heis.h"

static const unsigned long DOR_TID = 3000;   // time the door stays open [ms]

// ---------------------------------------------------------------------------
// Constructor and setup
// ---------------------------------------------------------------------------
Heis::Heis() {
  _tilstand = STAAR;
  _maalEtasje = 0;
  _tidInn = 0;
  _retning = 0;
  _videre = 0;
}

void Heis::begin() {
  _ko.begin();
}

// ---------------------------------------------------------------------------
// Change state, store the time and remember the travel direction
// ---------------------------------------------------------------------------
void Heis::byttTil(Tilstand ny) {
  _tilstand = ny;
  _tidInn = millis();

  if (ny == KJORER_OPP) _retning = 1;
  if (ny == KJORER_NED) _retning = -1;
}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------
int Heis::maalEtasje() {
  return _maalEtasje;
}

Heis::Tilstand Heis::tilstand() {
  return _tilstand;
}

// ---------------------------------------------------------------------------
// One step of the state machine, called every 20 ms
// ---------------------------------------------------------------------------
void Heis::oppdater(int etasjeNaa, long pos, Door& dor, bool overvekt) {
  // Read buttons, joystick and encoder into the queue
  _ko.oppdater();

  // The cabin has arrived when it is within TOLERANSE counts of the target
  bool fremme = abs(pos - etasjePosisjon(_maalEtasje)) < TOLERANSE;
  unsigned long tidITilstand = millis() - _tidInn;

  switch (_tilstand) {
    // Standing still: open the door for a request here or overload,
    // otherwise drive to the next stop above or below
    case STAAR:
      {
        if (!dor.isClosed()) {
          break;
        }
        int over = _ko.nesteStoppOver(etasjeNaa);
        int under = _ko.nesteStoppUnder(etasjeNaa);

        if (_ko.erBestilt(etasjeNaa) || overvekt) {
          _videre = 0;
          dor.openDoor();
          byttTil(DOR_AAPNER);
        } else if (over != -1) {
          _maalEtasje = over;
          byttTil(KJORER_OPP);
        } else if (under != -1) {
          _maalEtasje = under;
          byttTil(KJORER_NED);
        } else {
          _maalEtasje = etasjeNaa;
        }
        break;
      }
    // Moving up: move the target to a closer stop if a new request comes in,
    // and open the door when the cabin has arrived
    case KJORER_OPP:
      {
        int over = _ko.nesteStoppOver(etasjeNaa);

        if (!fremme && over != -1 && (over < _maalEtasje || !_ko.skalBetjenes(_maalEtasje, 1))) {
          _maalEtasje = over;
        }

        if (fremme) {
          int videre;
          if (over != -1) {
            videre = 1;
          } else {
            videre = 0;
          }
          _videre = videre;
          dor.openDoor();
          byttTil(DOR_AAPNER);
        }
        break;
      }

    // Moving down: same as moving up, but in the other direction
    case KJORER_NED:
      {
        int under = _ko.nesteStoppUnder(etasjeNaa);

        if (!fremme && under != -1 && (under > _maalEtasje || !_ko.skalBetjenes(_maalEtasje, -1))) {
          _maalEtasje = under;
        }

        if (fremme) {
          int videre;
          if (under != -1) {
            videre = -1;
          } else {
            videre = 0;
          }
          _videre = videre;
          dor.openDoor();
          byttTil(DOR_AAPNER);
        }
        break;
      }

    // Door open: a new request here or overload restarts the timer,
    // otherwise the door closes after DOR_TID
    case DOR_AAPEN:
      {
        if (_ko.skalBetjenes(etasjeNaa, _retning)) {
          _ko.fjernBestilling(etasjeNaa, _retning);
          byttTil(DOR_AAPEN);
          break;
        }
        if (overvekt) {
          byttTil(DOR_AAPEN);
          break;
        }
        if (tidITilstand < DOR_TID) {
          break;
        }
        dor.closeDoor();
        byttTil(DOR_LUKKER);
        break;
      }

    // Door opening: clear the requests at this floor when the door is fully open
    case DOR_AAPNER:
      {
        if (dor.isOpen()) {
          _ko.fjernBestilling(etasjeNaa, _videre);
          byttTil(DOR_AAPEN);
        }
        break;
      }

    // Door closing: reopen for a new request here or overload. When the door
    // is closed, continue in the same direction, or stop if nothing is ahead
    case DOR_LUKKER:
      {
        if (_ko.skalBetjenes(etasjeNaa, _retning) || overvekt) {
          _videre = _retning;
          dor.openDoor();
          byttTil(DOR_AAPNER);
          break;
        }
        if (!dor.isClosed()) {
          break;
        }


        int over = _ko.nesteStoppOver(etasjeNaa);
        int under = _ko.nesteStoppUnder(etasjeNaa);

        if (_retning == 1 && over != -1) {
          _maalEtasje = over;
          byttTil(KJORER_OPP);
        } else if (_retning == -1 && under != -1) {
          _maalEtasje = under;
          byttTil(KJORER_NED);
        } else {
          byttTil(STAAR);
        }
        break;

        break;
      }
  }
}

// ---------------------------------------------------------------------------
// Pass-through to the queue (menu, emergency mode and debug output)
// ---------------------------------------------------------------------------
int Heis::valgtEtasje() {
  return _ko.valgtEtasje();
}

int Heis::valgtRetning() {
  return _ko.valgtRetning();
}

bool Heis::visMeny() {
  return _ko.visMeny();
}

int Heis::count() {
  return _ko.count();
}

bool Heis::EMERGENCY() {
  return _ko.EMERGENCY();
}
