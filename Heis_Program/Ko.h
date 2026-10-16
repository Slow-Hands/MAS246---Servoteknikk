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
    bool erBestilt(int etasje);
   void fjernBestilling(int etasje);
   int  nesteOver(int etasje);     // nærmeste bestilte etasje over, eller -1
   int  nesteUnder(int etasje);    // nærmeste bestilte etasje under, eller -1

  private:
    int _maalEtasje;
    bool _forrigeKnapp[ANTALL_ETASJER];
    bool _bestilt[ANTALL_ETASJER];
};

#endif