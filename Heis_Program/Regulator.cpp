// =============================================================================
// Regulator.cpp - PID controller and motor output
// =============================================================================

#include "Regulator.h"
#include "Posisjon.h"

// ---------------------------------------------------------------------------
// Controller gains and limits
// ---------------------------------------------------------------------------
static const float KP = 0.4, KI = 0.35, KD = 0.12;
static const float ALFA = 0.3;          // low-pass filter factor for speed and D-term
static const float I_GRENSE = 30;       // limit of the integral (anti-windup)
static const int FART_MAX = 120;        // maximum PWM (out of 255)
static const int FART_MIN = 0;

// ---------------------------------------------------------------------------
// Constructor and pin setup
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// One controller step, called every 20 ms
// ---------------------------------------------------------------------------
void Regulator::oppdater(long feil, long pos, float dt, bool ferdig) {
  // Measured speed, low-pass filtered (used for logging)
  float fart = (pos - _forrigePos) / dt;
  _forrigePos = pos;
  _fartFilt = ALFA * fart + (1 - ALFA) * _fartFilt;

  // Derivative of the error, low-pass filtered
  float dFeil = (feil - _forrigeFeil) / dt;
  _forrigeFeil = feil;
  _dFilt = ALFA * dFeil + (1 - ALFA) * _dFilt;

  // PID control signal
  float u = KP * feil + KI * _integral + KD * _dFilt;

  // Anti-windup: do not integrate when the output is saturated and the error
  // pushes in the same direction. The integral is also limited to +-I_GRENSE.
  bool mettet = abs(u) > FART_MAX;
  bool sammeRetning = feil * u > 0;
  if (!(mettet && sammeRetning)) {
    _integral += feil * dt;
    _integral = constrain(_integral, -I_GRENSE, I_GRENSE);
  }

  // Motor output: off when the reference is finished and the cabin is within
  // the tolerance, otherwise the sign gives the direction and |u| the PWM
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

// ---------------------------------------------------------------------------
// Write direction and PWM to the H-bridge
// ---------------------------------------------------------------------------
void Regulator::kjoerMotor(int retning, int pwm) {
  if (retning == 1) {
    digitalWrite(_retningPin, HIGH);
  } else {
    digitalWrite(_retningPin, LOW);
  }
  analogWrite(_fartPin, pwm);
}
