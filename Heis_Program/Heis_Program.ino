#include "Posisjon.h"
#include "Display.h"
#include "Regulator.h"
#include "Heis.h"
#include "Profil.h"

Display display;
Regulator regulator(6, 7);
Heis heis;

Profil profil(1500, 2000);

unsigned long forrigeTid = 0;
// Logging for heislogg.py
bool forrigeFerdig = true;          
unsigned long turStart = 0;         
int etterlogg = 0;                  
const int ETTERLOGG_SAMPLER = 100;  

void setup() {
  Serial.begin(115200);

  posisjonBegin();
  profil.start(lesPosisjon());
  regulator.begin();
  display.begin();
  heis.begin();

  forrigeTid = millis();
}

void loop() {
  unsigned long naa = millis();
  if (naa - forrigeTid < 20) return;
  float dt = (naa - forrigeTid) / 1000.0;
  forrigeTid = naa;

  long pos = lesPosisjon();
  int etasjeNaa = naermesteEtasje(pos);

  heis.oppdater(etasjeNaa, pos);

  long maalPos = etasjePosisjon(heis.maalEtasje());
  long avstand = maalPos - pos;

  long ref = profil.oppdater(maalPos, dt);
  bool ferdig = (ref == maalPos);
  long folgefeil = ref - pos;

  int pil;
  if (avstand > TOLERANSE) {
    pil = 1;
  } else if (avstand < -TOLERANSE) {
    pil = -1;
  } else {
    pil = 0;
  }

  if (heis.EMERGENCY()) {
    display.EMERGENCY();
  } 
  else {
    if (heis.visMeny()) {
      display.visMeny(heis.valgtEtasje(), heis.valgtRetning());
    } else {
      display.vis(etasjeNaa, pil);
    }
  }

  regulator.oppdater(folgefeil, pos, dt, ferdig);

  Serial.print(ref);
  Serial.print(" ");
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(folgefeil);
  Serial.print(" ");
  Serial.print(regulator.fart());
  Serial.print(" ");
  Serial.print(heis.count());
  Serial.print(" ");
  Serial.println(heis.tilstand());


  // ---------- Logging for heislogg.py ----------
  // En tur starter når profilen går fra ferdig til ikke ferdig (nytt mål)
  if (!ferdig && forrigeFerdig) {
    turStart = naa;
    Serial.println("t_ms,ref,pos,v");
  }

  if (!ferdig) {
    etterlogg = ETTERLOGG_SAMPLER;  // hold loggen åpen så lenge turen pågår
  }

  if (!ferdig || etterlogg > 0) {
    Serial.print(naa - turStart);  Serial.print(',');
    Serial.print(ref);             Serial.print(',');
    Serial.print(pos);             Serial.print(',');
    Serial.println(regulator.fart());
    if (ferdig) etterlogg--;        // tell ned de ekstra samplene etter turen
  }

  forrigeFerdig = ferdig;
}