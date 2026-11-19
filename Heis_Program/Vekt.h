// =============================================================================
// Vekt.h - simulated passenger load
//
// The load is simulated with a potentiometer and scaled to 0-5000 kg.
// =============================================================================

#ifndef VEKT_H
#define VEKT_H

#include <Arduino.h>

class Vekt {
  public:
    Vekt(int pin, int grense);
    bool overvekt();   // true when the load is at or above the limit
    int verdi();       // raw analog value 0-1023
    int kg();          // load in kg

  private:
    int _pin;          // analog input of the potentiometer
    int _grense;       // overload limit [kg]
};

#endif
