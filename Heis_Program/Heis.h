#ifndef HEIS_H
#define HEIS_H

#include <Arduino.h>
#include "Ko.h"

class Heis {
  public:
    enum Tilstand { STAAR, KJORER_OPP, KJORER_NED, DOR_AAPEN };

    Heis();
    void begin();
    void oppdater(int etasjeNaa, long pos);
    int maalEtasje();
    Tilstand tilstand();
    int valgtEtasje();
    int valgtRetning();
    bool visMeny();
    


  private:
    void byttTil(Tilstand ny);

    Ko _ko;
    Tilstand _tilstand;
    int _maalEtasje;
    unsigned long _tidInn;
    int _retning;      
};

#endif