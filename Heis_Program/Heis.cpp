#include "Heis.h"

static const unsigned long DOR_TID = 3000;

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

void Heis::oppdater(int etasjeNaa, long pos, Door& dor, bool overvekt) {
  _ko.oppdater();

  bool fremme = abs(pos - etasjePosisjon(_maalEtasje)) < TOLERANSE;
  unsigned long tidITilstand = millis() - _tidInn;

  switch (_tilstand) {
    case STAAR:
      {
        if (!dor.isClosed() || overvekt) {   //
          break;
        }
        int over = _ko.nesteStoppOver(etasjeNaa);
        int under = _ko.nesteStoppUnder(etasjeNaa);

        if (_ko.erBestilt(etasjeNaa)) {
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
    case KJORER_OPP:
      {
        int over = _ko.nesteStoppOver(etasjeNaa);

        if (over != -1 && over < _maalEtasje) {
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

    case KJORER_NED:
      {
        int under = _ko.nesteStoppUnder(etasjeNaa);

        if (under != -1 && under > _maalEtasje) {
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

    case DOR_AAPNER:
      {
        if (dor.isOpen()) {
          _ko.fjernBestilling(etasjeNaa, _videre);
          byttTil(DOR_AAPEN);
        }
        break;
      }

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

int Heis::valgtEtasje() {
  return _ko.valgtEtasje();
}

int Heis::valgtRetning() {
  return _ko.valgtRetning();
}

bool Heis::visMeny() {
  return _ko.visMeny();
}
