// =============================================================================
// Profil.h - S-curve motion profile
//
// Generates the position reference in two steps: a trapezoidal velocity ramp,
// smoothed by a moving average, which gives an S-curve with limited jerk.
// =============================================================================

#ifndef PROFIL_H
#define PROFIL_H

#include <Arduino.h>

class Profil {
  public:
    Profil(float maksFart, float maksAkselerasjon);
    void start(long pos);                      // start the profile at the current position
    long oppdater(long maalPos, float dt);     // next reference sample [counts]
    float fart();                              // ramp velocity [counts/s]

  private:
    // Trapezoidal ramp
    float _vMax;
    float _aMax;
    float _rampePos;
    float _rampeFart;

    // Moving average that turns the trapezoidal profile into an S-curve
    static const int N = 12;   // window: 12 x 20 ms = 0.24 s  ->  jerk = aMax / 0.24
    long _buffer[N];           // the last N trapezoidal positions
    long _sum;                 // sum of all values in _buffer
    int _indeks;               // index of the oldest value
};

#endif
