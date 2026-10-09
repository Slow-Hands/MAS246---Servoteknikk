#include "Posisjon.h"
#include "Display.h"
#include "Regulator.h"
#include "Ko.h"

Display display;
Regulator regulator(6, 7);
Ko ko;

unsigned long forrigeTid = 0;

void setup() {
  Serial.begin(115200);

  posisjonBegin();
  regulator.begin();
  display.begin();
  ko.begin();

  forrigeTid = millis();
}

void loop() {
  unsigned long naa = millis();
  if (naa - forrigeTid < 20) return;
  float dt = (naa - forrigeTid) / 1000.0;
  forrigeTid = naa;

  ko.oppdater();

  long pos = lesPosisjon();
  int etasjeNaa = naermesteEtasje(pos);

  long maalPos = etasjePosisjon(ko.maalEtasje());
  long avstand = maalPos - pos;

  int pil;
  if (avstand > TOLERANSE) {
    pil = 1;
  } else if (avstand < -TOLERANSE) {
    pil = -1;
  } else {
    pil = 0;
  }

  display.vis(etasjeNaa, pil);
  regulator.oppdater(avstand, pos, dt);

  Serial.print(maalPos);
  Serial.print(" ");
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(regulator.fart());
  Serial.print(" ");
  Serial.println(etasjeNaa);
}