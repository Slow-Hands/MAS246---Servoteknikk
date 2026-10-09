#ifndef REGULATOR_H
#define REGULATOR_H

#include <Arduino.h>

class Regulator {
  public:
    Regulator(int retningPin, int fartPin);
    void begin();
    void oppdater(long avstand, long pos, float dt);
    float fart();

  private:
    void kjoerMotor(int retning, int pwm);

    int _retningPin;
    int _fartPin;
    float _integral;
    float _forrigePos;
    float _fartFilt;
};

#endif