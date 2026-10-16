#include "Heis.h"

static const unsigned long DOR_TID = 3000;  // ms døren står åpen

Heis::Heis() {
  _tilstand = STAAR;
  _maalEtasje = 0;
  _tidInn = 0;
  _retning = 0;
}

void Heis::begin() {
  _ko.begin();
}

void Heis::byttTil(Tilstand ny) {
  _tilstand = ny;
  _tidInn = millis();

  if (ny == KJORER_OPP) _retning = 1;
  if (ny == KJORER_NED) _retning = -1;
}

int Heis::maalEtasje() {
  return _maalEtasje;
}

Heis::Tilstand Heis::tilstand() {
  return _tilstand;
}

void Heis::oppdater(int etasjeNaa, long pos) {
  _ko.oppdater();

  bool fremme = abs(pos - etasjePosisjon(_maalEtasje)) < TOLERANSE;
  unsigned long tidITilstand = millis() - _tidInn;

  switch (_tilstand) {
    case STAAR:
      {
        int over = _ko.nesteOver(etasjeNaa);
        int under = _ko.nesteUnder(etasjeNaa);

        if (_ko.erBestilt(etasjeNaa)) {
          _ko.fjernBestilling(etasjeNaa);
          byttTil(DOR_AAPEN);
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
    case KJORER_OPP:
      {
        int over = _ko.nesteOver(etasjeNaa);  // nærmeste bestilling over der heisen er

        if (over != -1 && over < _maalEtasje) {  // fant noe, OG det er nærmere enn målet?
          _maalEtasje = over;
        }

        if (fremme) {
          _ko.fjernBestilling(etasjeNaa);  // fjern bestillingen i målet
          byttTil(DOR_AAPEN);              // bytt til DOR_AAPEN
        }
        break;
      }

    case KJORER_NED:
      {
        int under = _ko.nesteUnder(etasjeNaa);  // nærmeste bestilling over der heisen er

        if (under != -1 && under > _maalEtasje) {  // fant noe, OG det er nærmere enn målet?
          _maalEtasje = under;
        }

        if (fremme) {
          _ko.fjernBestilling(etasjeNaa);  // fjern bestillingen i målet
          byttTil(DOR_AAPEN);              // bytt til DOR_AAPEN
        }
        break;
      }

    case DOR_AAPEN:
      {
        if (tidITilstand < DOR_TID) {  // døren har ikke stått åpen lenge nok
          break;                   // gjør ingenting denne runden
        }

        int over = _ko.nesteOver(etasjeNaa);
        int under = _ko.nesteUnder(etasjeNaa);

        if (_retning == 1 && over != -1) {  // kjørte opp, og flere over?
          _maalEtasje = over;
          byttTil(KJORER_OPP);
        } else if (_retning == -1 && under != -1) {  // kjørte ned, og flere under?
          _maalEtasje = under;
          byttTil(KJORER_NED);
        } else {  // ingenting mer i samme retning
          byttTil(STAAR);
        }
        break;
      }
  }
}