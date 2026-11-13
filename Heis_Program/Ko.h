#ifndef KO_H
#define KO_H

#include <Arduino.h>
#include "Posisjon.h"

class Ko {
  public:
    Ko();
    void begin();
    void oppdater();
    bool erBestilt(int etasje);
    bool skalBetjenes(int etasje, int retning);
    void fjernBestilling(int etasje, int retning);
    int  nesteStoppOver(int etasje);
    int  nesteStoppUnder(int etasje);
    void leggTilUtvendig(int etasje, int retning);
    int  valgtEtasje();
    int  valgtRetning();
    bool visMeny();
    int  count();
    bool EMERGENCY();

  private:
    bool _forrigeKnapp[ANTALL_ETASJER];
    bool _bestiltInne[ANTALL_ETASJER];
    bool _bestiltOpp[ANTALL_ETASJER];    
    bool _bestiltNed[ANTALL_ETASJER];    
    bool _bestiltEMERGENCY[ANTALL_ETASJER]; 
    int  _valgtEtasje;
    int  _valgtRetning;
    int  _EMERGENCYcount;
    int  _resetEMERGENCY;
    bool _setEMERGENCY;
    bool _joyXVarMidt;
    bool _forrigeEncKnapp;
    bool _visMeny;
    bool _naaEncA;
    bool _naaEncB;
    bool _forrigeEncA;
    bool _forrigeEncB;
};
#endif