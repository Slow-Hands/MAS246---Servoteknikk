#include "Regulator.h"
#include "Posisjon.h"

static const float KP = 0.2, KI = 0.35, KD = 0.12;
static const float ALFA = 0.3;
static const int FART_MAX = 120;
static const int FART_MIN = 0;

Regulator::Regulator(int retningPin, int fartPin) {
  _retningPin = retningPin;
  _fartPin = fartPin;
  _integral = 0;
  _forrigePos = 0;
  _fartFilt = 0;
}

void Regulator::begin() {
  pinMode(_retningPin, OUTPUT);
  pinMode(_fartPin, OUTPUT);
}

void Regulator::oppdater(long avstand, long pos, float dt) {
  float fart = (pos - _forrigePos) / dt;
  _forrigePos = pos;
  _fartFilt = ALFA * fart + (1 - ALFA) * _fartFilt;

  float u = KP * avstand + KI * _integral - KD * _fartFilt;

  bool mettet = abs(u) > FART_MAX;
  bool sammeRetning = avstand * u > 0;
  if (!(mettet && sammeRetning)) {
    _integral += avstand * dt;
    _integral = constrain(_integral, -30, 30);
  }

  int retning;
  int pwm;
  if (abs(avstand) < TOLERANSE) {
    retning = 0;
    pwm = 0;
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