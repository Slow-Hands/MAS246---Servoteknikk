#include "Vekt.h"

Vekt::Vekt(int pin, int grense) {
  _pin = pin;
  _grense = grense;
}

int Vekt::verdi() {
  return analogRead(_pin);
}

int Vekt::kg() {
  return (long)verdi() * 5000L / 1023;   
}

bool Vekt::overvekt() {
  return kg() >= _grense;
}