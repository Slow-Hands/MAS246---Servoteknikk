#include "Posisjon.h"

static const int ENCODER_A = 21;
static const int ENCODER_B = 20;

static volatile long encoderPos = 0;

static const long etasjeTerskel[ANTALL_ETASJER] = { 0, 4000, 8000, 12000, 16000, 20000, 24000, 28000 };

static void tellEncoder() {
  if (digitalRead(ENCODER_A) == digitalRead(ENCODER_B)) encoderPos--;
  else encoderPos++;
}

void posisjonBegin() {
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), tellEncoder, CHANGE);
}

long lesPosisjon() {
  noInterrupts();
  long pos = encoderPos;
  interrupts();
  return pos;
}

int naermesteEtasje(long pos) {
  int beste = 0;
  long minsteAvvik = abs(pos - etasjeTerskel[0]);
  for (int i = 1; i < ANTALL_ETASJER; i++) {
    long avvik = abs(pos - etasjeTerskel[i]);
    if (avvik < minsteAvvik) {
      minsteAvvik = avvik;
      beste = i;
    }
  }
  return beste;
}

long etasjePosisjon(int etasje) {
  return etasjeTerskel[etasje];
}