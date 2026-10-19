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
};

#endif