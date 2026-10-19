#include "Profil.h"

Profil::Profil(float maksFart, float maksAkselerasjon) {
  _vMax = maksFart;
  _aMax = maksAkselerasjon;
  _rampePos = 0;
  _rampeFart = 0;
}

void Profil::start(long pos) {
  _rampePos = pos;
  _rampeFart = 0;
}

long Profil::oppdater(long maalPos, float dt) {
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

  float steg = _rampeFart * dt;
  if (abs(steg) >= abs(gjenstaar) && gjenstaar * _rampeFart >= 0) {
    _rampePos = maalPos;
    _rampeFart = 0;
  } else {
    _rampePos += steg;
  }

  return (long)_rampePos;
}

float Profil::fart() {
  return _rampeFart;
}