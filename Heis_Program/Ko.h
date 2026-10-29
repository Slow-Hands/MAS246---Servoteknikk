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
   int valgtEtasje();
    int valgtRetning();
    bool visMeny();

  private:
    bool _forrigeKnapp[ANTALL_ETASJER];
    bool _bestiltInne[ANTALL_ETASJER];
    bool _bestiltOpp[ANTALL_ETASJER];    
    bool _bestiltNed[ANTALL_ETASJER]; 
    int _valgtEtasje;
    int _valgtRetning;
    bool _joyXVarMidt;
    bool _forrigeEncKnapp;  
    bool _visMeny;
};
#endif