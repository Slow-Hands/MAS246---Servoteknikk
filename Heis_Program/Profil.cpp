// =============================================================================
// Profil.cpp - S-curve motion profile
// =============================================================================

#include "Profil.h"

// ---------------------------------------------------------------------------
// Constructor: store the speed and acceleration limits
// ---------------------------------------------------------------------------
Profil::Profil(float maksFart, float maksAkselerasjon) {
  _vMax = maksFart;
  _aMax = maksAkselerasjon;
  _rampePos = 0;
  _rampeFart = 0;
  _sum = 0;
  _indeks = 0;
}

// ---------------------------------------------------------------------------
// Start the profile at the current position
// ---------------------------------------------------------------------------
void Profil::start(long pos) {
  _rampePos = pos;
  _rampeFart = 0;

  // Fill the buffer with the start position so the average starts where the cabin is
  for (int i = 0; i < N; i++) {
    _buffer[i] = pos;
  }
  _sum = pos * N;
  _indeks = 0;
}

// ---------------------------------------------------------------------------
// Next reference sample towards the target position
// ---------------------------------------------------------------------------
long Profil::oppdater(long maalPos, float dt) {
  // Step 1: trapezoidal ramp. Accelerate towards vMax while the remaining
  // distance is longer than the braking distance v^2 / (2 aMax),
  // otherwise brake with aMax.
  float gjenstaar = maalPos - _rampePos;
  float bremselengde = (_rampeFart * _rampeFart) / (2 * _aMax);

  if (gjenstaar * _rampeFart >= 0 && abs(gjenstaar) > bremselengde) {
    if (gjenstaar > 0) {
      _rampeFart += _aMax * dt;
    } else {
      _rampeFart -= _aMax * dt;
    }
    _rampeFart = constrain(_rampeFart, -_vMax, _vMax);
  } else {
    float dv = _aMax * dt;
    if (_rampeFart > dv) {
      _rampeFart -= dv;
    } else if (_rampeFart < -dv) {
      _rampeFart += dv;
    } else {
      _rampeFart = 0;
    }
  }

  // Move the ramp position, and stop exactly at the target if the next
  // step would pass it
  float steg = _rampeFart * dt;
  if (abs(steg) >= abs(gjenstaar) && gjenstaar * _rampeFart >= 0) {
    _rampePos = maalPos;
    _rampeFart = 0;
  } else {
    _rampePos += steg;
  }

  // Step 2: moving average over the last N ramp positions (S-curve)
  long raa = (long)_rampePos;           // the trapezoidal reference

  _sum += raa - _buffer[_indeks];       // add the new value, subtract the oldest
  _buffer[_indeks] = raa;               // the new value replaces the oldest
  _indeks = (_indeks + 1) % N;          // next index, back to 0 after the last

  return _sum / N;                      // smoothed reference = S-curve
}

float Profil::fart() {
  return _rampeFart;
}
