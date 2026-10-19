#include "Posisjon.h"
#include "Display.h"
#include "Regulator.h"
#include "Heis.h"
#include "Profil.h"

Display display;
Regulator regulator(6, 7);
Heis heis;

Profil profil(2000, 3000);

unsigned long forrigeTid = 0;

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

  heis.oppdater(etasjeNaa,pos);

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

  display.vis(etasjeNaa, pil);
  regulator.oppdater(folgefeil, pos, dt, ferdig);

  Serial.print(ref);
  Serial.print(" ");
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(folgefeil);
  Serial.print(" ");
  Serial.print(regulator.fart());
  Serial.print(" ");
  Serial.println(heis.tilstand());
}