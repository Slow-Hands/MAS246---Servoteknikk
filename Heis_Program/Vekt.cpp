// =============================================================================
// Vekt.cpp - simulated passenger load
// =============================================================================

#include "Vekt.h"

Vekt::Vekt(int pin, int grense) {
  _pin = pin;
  _grense = grense;
}

// Raw reading of the potentiometer
int Vekt::verdi() {
  return analogRead(_pin);
}

// Scale 0-1023 to 0-5000 kg
int Vekt::kg() {
  return (long)verdi() * 5000L / 1023;
}

// Overload when the load reaches the limit
bool Vekt::overvekt() {
  return kg() >= _grense;
}
