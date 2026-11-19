// =============================================================================
// Regulator.h - PID controller and motor output
//
// Discrete PID controller without feedforward. Drives the DC motor through
// the H-bridge with a direction pin and a PWM pin.
// =============================================================================

#ifndef REGULATOR_H
#define REGULATOR_H

#include <Arduino.h>

class Regulator {
  public:
    Regulator(int retningPin, int fartPin);
    void begin();
    void oppdater(long feil, long pos, float dt, bool ferdig);   // one controller step
    float fart();                                                // filtered measured speed [counts/s]

  private:
    void kjoerMotor(int retning, int pwm);

    int _retningPin;       // direction pin of the H-bridge
    int _fartPin;          // PWM pin of the H-bridge
    float _integral;       // integral of the error
    float _forrigePos;     // last position, for the speed estimate
    float _fartFilt;       // low-pass filtered speed
    float _forrigeFeil;    // last error, for the D-term
    float _dFilt;          // low-pass filtered derivative of the error
};

#endif
