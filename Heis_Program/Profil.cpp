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
    if (_rampeFart > 0) {                                         
      _rampeFart -= _aMax * dt;
    } else {
      _rampeFart += _aMax * dt;
    }
  }

  _rampePos += _rampeFart * dt;                                   

  if (abs(maalPos - _rampePos) < 1 && abs(_rampeFart) < _aMax * dt) {  
    _rampePos = maalPos;                                          
    _rampeFart = 0;                                               
  }

  return (long)_rampePos;                                         
}