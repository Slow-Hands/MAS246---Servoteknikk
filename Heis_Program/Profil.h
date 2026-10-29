#ifndef PROFIL_H
#define PROFIL_H

#include <Arduino.h>

class Profil {
  public:
    Profil(float maksFart, float maksAkselerasjon);
    void start(long pos);
    long oppdater(long maalPos, float dt);
    float fart();

  private:
    float _vMax;
    float _aMax;
    float _rampePos;
    float _rampeFart;

    // Glidende gjennomsnitt som gjør trapesprofilen om til en S-kurve
    static const int N = 12;   // vindu: 12 × 20 ms = 0,24 s  ->  rykk ≈ aMax / 0,24
    long _buffer[N];           // de N siste trapes-posisjonene
    long _sum;                 // summen av alt i _buffer
    int _indeks;               // plassen med den eldste verdien
};

#endif