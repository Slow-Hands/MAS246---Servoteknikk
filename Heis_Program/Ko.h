#ifndef KO_H
#define KO_H

#include <Arduino.h>
#include "Posisjon.h"

class Ko {
  public:
    Ko();
    void begin();
    void oppdater();
    int maalEtasje();

  private:
    int _maalEtasje;
    bool _forrigeKnapp[ANTALL_ETASJER];
    bool _ledPaa[ANTALL_ETASJER];
};

#endif