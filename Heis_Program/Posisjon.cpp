// =============================================================================
// Posisjon.cpp - encoder reading and conversion between counts and floors
// =============================================================================

#include "Posisjon.h"

// ---------------------------------------------------------------------------
// Encoder pins and position counter
// ---------------------------------------------------------------------------
static const int ENCODER_A = 21;   // channel A, triggers the interrupt
static const int ENCODER_B = 20;   // channel B, gives the direction

static volatile long encoderPos = 0;   // updated by the interrupt, so volatile

// Position of each floor in encoder counts (4000 counts per floor)
static const long etasjeTerskel[ANTALL_ETASJER] = { 0, 4000, 8000, 12000, 16000, 20000, 24000, 28000 };

// ---------------------------------------------------------------------------
// Interrupt on every edge of channel A. Comparing A and B gives the
// direction, so the count goes up or down (2x decoding).
// ---------------------------------------------------------------------------
static void tellEncoder() {
  if (digitalRead(ENCODER_A) == digitalRead(ENCODER_B)) encoderPos--;
  else encoderPos++;
}

void posisjonBegin() {
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), tellEncoder, CHANGE);
}

// ---------------------------------------------------------------------------
// Read the position. Interrupts are turned off while the 32-bit value is
// copied, so it cannot change halfway through the read.
// ---------------------------------------------------------------------------
long lesPosisjon() {
  noInterrupts();
  long pos = encoderPos;
  interrupts();
  return pos;
}

// ---------------------------------------------------------------------------
// Conversion between position and floor. The nearest floor changes when the
// cabin passes the midpoint (2000 counts) between two floors.
// ---------------------------------------------------------------------------
int naermesteEtasje(long pos) {
  int beste = (pos + 2000) / 4000;
  return beste;
}

long etasjePosisjon(int etasje) {
  return etasjeTerskel[etasje];
}
