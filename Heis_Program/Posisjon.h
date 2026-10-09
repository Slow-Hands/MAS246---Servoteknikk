#ifndef POSISJON_H
#define POSISJON_H

#include <Arduino.h>

const int ANTALL_ETASJER = 8;
const int TOLERANSE = 5;

void posisjonBegin();
long lesPosisjon();
int naermesteEtasje(long pos);
long etasjePosisjon(int etasje);

#endif