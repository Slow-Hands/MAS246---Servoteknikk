#ifndef VEKT_H
#define VEKT_H

#include <Arduino.h>

class Vekt {
  public:
    Vekt(int pin, int grense);
    bool overvekt();
    int verdi();
    int kg();

  private:
    int _pin;
    int _grense;
};

#endif