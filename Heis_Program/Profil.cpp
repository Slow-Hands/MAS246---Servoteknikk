#include "Profil.h"

Profil::Profil(float maksFart, float maksAkselerasjon) {
  _vMax = maksFart;
  _aMax = maksAkselerasjon;
  _rampePos = 0;
  _rampeFart = 0;
  _sum = 0;
  _indeks = 0;
}

void Profil::start(long pos) {
  _rampePos = pos;
  _rampeFart = 0;

  // Fyll bufferen med startposisjonen så snittet starter der heisen står
  for (int i = 0; i < N; i++) {
    _buffer[i] = pos;
  }
  _sum = pos * N;
  _indeks = 0;
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

  long raa = (long)_rampePos;           // trapesreferansen som før

  _sum += raa - _buffer[_indeks];       // legg til ny verdi, trekk fra eldste
  _buffer[_indeks] = raa;               // ny verdi tar plassen til eldste
  _indeks = (_indeks + 1) % N;          // neste plass, tilbake til 0 etter siste

  return _sum / N;                      // glattet referanse = S-kurve
}

float Profil::fart() {
  return _rampeFart;
}