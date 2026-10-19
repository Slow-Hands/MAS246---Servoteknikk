#include "Regulator.h"
#include "Posisjon.h"

static const float KP = 0.2, KI = 0.35, KD = 0.12;
static const float ALFA = 0.3;
static const float I_GRENSE = 30;
static const int FART_MAX = 120;
static const int FART_MIN = 0;

Regulator::Regulator(int retningPin, int fartPin) {
  _retningPin = retningPin;
  _fartPin = fartPin;
  _integral = 0;
  _forrigePos = 0;
  _fartFilt = 0;
  _forrigeFeil = 0;
  _dFilt = 0;
}

void Regulator::begin() {
  pinMode(_retningPin, OUTPUT);
  pinMode(_fartPin, OUTPUT);
}

void Regulator::oppdater(long feil, long pos, float dt, bool ferdig) {
  float fart = (pos - _forrigePos) / dt;
  _forrigePos = pos;
  _fartFilt = ALFA * fart + (1 - ALFA) * _fartFilt;

  float dFeil = (feil - _forrigeFeil) / dt;
  _forrigeFeil = feil;
  _dFilt = ALFA * dFeil + (1 - ALFA) * _dFilt;

  float u = KP * feil + KI * _integral + KD * _dFilt;

  bool mettet = abs(u) > FART_MAX;
  bool sammeRetning = feil * u > 0;
  if (!(mettet && sammeRetning)) {
    _integral += feil * dt;
    _integral = constrain(_integral, -I_GRENSE, I_GRENSE);
  }

  int retning;
  int pwm;
  if (ferdig && abs(feil) < TOLERANSE) {
    retning = 0;
    pwm = 0;
    _integral = 0;
  } else {
    if (u > 0) retning = 1;
    else retning = -1;
    pwm = abs(u);
    pwm = constrain(pwm, FART_MIN, FART_MAX);
  }

  kjoerMotor(retning, pwm);
}

float Regulator::fart() {
  return _fartFilt;
}

void Regulator::kjoerMotor(int retning, int pwm) {
  if (retning == 1) {
    digitalWrite(_retningPin, HIGH);
  } else {
    digitalWrite(_retningPin, LOW);
  }
  analogWrite(_fartPin, pwm);
}