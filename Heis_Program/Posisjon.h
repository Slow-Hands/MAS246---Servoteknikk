// =============================================================================
// Posisjon.h - cabin position from the encoder
//
// Not a class: a module with constants and free functions. The position is
// counted in encoder counts, and 4000 counts is one floor (4 m in model scale).
// =============================================================================

#ifndef POSISJON_H
#define POSISJON_H

#include <Arduino.h>

const int ANTALL_ETASJER = 8;   // floors 0-7
const int TOLERANSE = 5;        // the cabin is at the target within +-5 counts

void posisjonBegin();                   // start the encoder interrupt
long lesPosisjon();                     // current position [counts]
int naermesteEtasje(long pos);          // position -> nearest floor
long etasjePosisjon(int etasje);        // floor -> position [counts]

#endif
