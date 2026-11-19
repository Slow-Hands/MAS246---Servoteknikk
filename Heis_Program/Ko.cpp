// =============================================================================
// Ko.cpp - request queue
//
// Cabin calls come from the eight floor buttons. Hall calls are made with the
// joystick (x = floor, y = direction) and confirmed with the encoder button.
// Turning the rotary encoder eight steps starts the emergency mode, and two
// presses on the encoder button reset it.
// =============================================================================

#include "Ko.h"

// ---------------------------------------------------------------------------
// Pins and thresholds
// ---------------------------------------------------------------------------
static const int KNAPP_PINS[ANTALL_ETASJER] = { 22, 23, 24, 25, 26, 27, 28, 29 };   // floor buttons 0-7
static const int LED_PINS[ANTALL_ETASJER] = { 49, 48, 47, 46, 45, 44, 43, 42 };     // floor LEDs 0-7
static const int JOY_X_PIN = { A1 };
static const int JOY_Y_PIN = { A2 };
static const int EncoderPIN = { 2 };        // encoder push button
static const int Encoder_A_PIN = { 18 };    // encoder channel A
static const int Encoder_B_PIN = { 19 };    // encoder channel B

// Joystick thresholds (analogRead 0-1023): outside 200/800 counts as a move,
// between 400 and 600 counts as the centre
static const int Joy_Hoy = 800;
static const int Joy_Lav = 200;
static const int Joy_Mid_Hoy = 600;
static const int Joy_Mid_Lav = 400;
static const int EMERGENCY_limit = 8;       // encoder steps that start the emergency mode

// ---------------------------------------------------------------------------
// Constructor: empty queue, no menu, emergency mode off
// ---------------------------------------------------------------------------
Ko::Ko() {
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    _forrigeKnapp[i] = false;
    _bestiltInne[i] = false;
    _bestiltOpp[i] = false;
    _bestiltNed[i] = false;
    _bestiltEMERGENCY[i] = false;
  }
  _valgtEtasje = 0;
  _valgtRetning = 0;
  _EMERGENCYcount = 0;
  _resetEMERGENCY = 0;
  _joyXVarMidt = false;
  _forrigeEncKnapp = false;
  _setEMERGENCY = false;
  _naaEncA = digitalRead(Encoder_A_PIN);
  _naaEncB = digitalRead(Encoder_B_PIN);
  _forrigeEncA = digitalRead(Encoder_A_PIN);
  _forrigeEncB = digitalRead(Encoder_B_PIN);
}

// ---------------------------------------------------------------------------
// Pin setup: LEDs as outputs (off), buttons with internal pull-up
// ---------------------------------------------------------------------------
void Ko::begin() {
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
    pinMode(KNAPP_PINS[i], INPUT_PULLUP);
  }
  pinMode(EncoderPIN, INPUT_PULLUP);
}

// ---------------------------------------------------------------------------
// Read all inputs, called every 20 ms
// ---------------------------------------------------------------------------
void Ko::oppdater() {
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    // Floor button: a cabin call is registered on the falling edge
    bool knappNaa = digitalRead(KNAPP_PINS[i]);

    if (knappNaa == LOW && _forrigeKnapp[i] == HIGH) {

      _bestiltInne[i] = true;
      digitalWrite(LED_PINS[i], HIGH);
    }

    // Rotary encoder: compare the two channels with the last reading
    // to count steps up or down
    _naaEncA = digitalRead(Encoder_A_PIN);
    _naaEncB = digitalRead(Encoder_B_PIN);

    if (_naaEncA != _forrigeEncA && _naaEncB == _forrigeEncB) {
      if (_naaEncA == _naaEncB) {
        _EMERGENCYcount ++;
      }
      else {
        _EMERGENCYcount --;
      }
    }
    else if (_naaEncA == _forrigeEncA && _naaEncB != _forrigeEncB) {
      if (_naaEncA == _naaEncB) {
        _EMERGENCYcount --;
      }
      else {
        _EMERGENCYcount ++;
      }
    }

    _forrigeEncA = _naaEncA;
    _forrigeEncB = _naaEncB;

    // Enough steps in either direction: emergency stop at every floor,
    // so the cabin stops at the next floor and keeps the door open
    if (abs(_EMERGENCYcount) >= EMERGENCY_limit) {
      for (int i = 0; i < ANTALL_ETASJER; i++) {
        _bestiltEMERGENCY[i] = true;
        digitalWrite(LED_PINS[i], LOW);
      }
      _setEMERGENCY = true;
    }



    _forrigeKnapp[i] = knappNaa;
  }

  // Joystick x-axis: step the selected floor once per move,
  // the joystick must return to the centre before the next step
  int x = analogRead(JOY_X_PIN);

  if (_joyXVarMidt) {
    if (x > Joy_Hoy) {
      _valgtEtasje++;
      _joyXVarMidt = false;
      _visMeny = true;

    } else if (x < Joy_Lav) {
      _valgtEtasje--;
      _joyXVarMidt = false;
      _visMeny = true;
    }
  }

  if (Joy_Mid_Lav < x && x < Joy_Mid_Hoy) {
    _joyXVarMidt = true;
  }

  _valgtEtasje = constrain(_valgtEtasje, 0, ANTALL_ETASJER - 1);

  // Joystick y-axis: select the direction of the hall call
  int y = analogRead(JOY_Y_PIN);

  if (y > Joy_Hoy) {
    _valgtRetning = 1;
    _visMeny = true;
  } else if (y < Joy_Lav) {
    _valgtRetning = -1;
    _visMeny = true;
  }



  // Encoder button (falling edge): confirm the hall call in normal mode,
  // or count presses towards reset in emergency mode. Two presses clear
  // all requests and turn the emergency mode off.
  bool encNaa = digitalRead(EncoderPIN);

  if (encNaa == LOW && _forrigeEncKnapp == HIGH) {
    if (_setEMERGENCY == false) {
      leggTilUtvendig(_valgtEtasje, _valgtRetning);
      _valgtRetning = 0;
      _visMeny = false;
    }
    else {
      _resetEMERGENCY ++;
      if (_resetEMERGENCY == 2) {
        _resetEMERGENCY = 0;
        _setEMERGENCY = false;
        _EMERGENCYcount = 0;
        for (int i = 0; i < ANTALL_ETASJER; i++) {
          _bestiltInne[i] = false;
          _bestiltOpp[i] = false;
          _bestiltNed[i] = false;
          _bestiltEMERGENCY[i] = false;
        }
      }
    }
  }

  _forrigeEncKnapp = encNaa;
}


// ---------------------------------------------------------------------------
// Is there any request at this floor?
// ---------------------------------------------------------------------------
bool Ko::erBestilt(int etasje) {
  return _bestiltInne[etasje] == true || _bestiltOpp[etasje] == true || _bestiltNed[etasje] == true || _bestiltEMERGENCY[etasje] == true;
}

// ---------------------------------------------------------------------------
// Clear the cabin call and the hall call in the given direction.
// Direction 0 clears both hall calls. The LED is turned off when the floor
// has no requests left.
// ---------------------------------------------------------------------------
void Ko::fjernBestilling(int etasje, int retning) {
  _bestiltInne[etasje] = false;

  if (retning == 1) {
    _bestiltOpp[etasje] = false;
  } else if (retning == -1) {
    _bestiltNed[etasje] = false;
  } else {
    _bestiltOpp[etasje] = false;
    _bestiltNed[etasje] = false;
  }

  if (!erBestilt(etasje)) {
    digitalWrite(LED_PINS[etasje], LOW);
  }
}

// ---------------------------------------------------------------------------
// Add a hall call. Calls up from the top floor, down from the bottom floor
// or without a direction are rejected.
// ---------------------------------------------------------------------------
void Ko::leggTilUtvendig(int etasje, int retning) {
  if (0 > etasje || etasje > ANTALL_ETASJER - 1 || retning == 1 && etasje >= ANTALL_ETASJER - 1 || retning == -1 && etasje <= 0 || retning == 0) {
    return;
  }

  if (retning == 1) {
    _bestiltOpp[etasje] = true;
  } else if (retning == -1) {
    _bestiltNed[etasje] = true;
  }

  digitalWrite(LED_PINS[etasje], HIGH);
}

// ---------------------------------------------------------------------------
// Next stop above: the first floor with a cabin call, an up call or an
// emergency stop. If there is none, the highest floor with a down call is
// returned, so the cabin goes to the last request before it turns around.
// Returns -1 if there are no requests above.
// ---------------------------------------------------------------------------
int Ko::nesteStoppOver(int etasje) {
  int snuEtasje = -1;
  for (int i = etasje + 1; i < ANTALL_ETASJER; i++) {
    if (_bestiltInne[i] || _bestiltOpp[i] || _bestiltEMERGENCY[i]) {
      return i;
    }
    if (erBestilt(i)) {
      snuEtasje = i;
    }
  }
  return snuEtasje;
}

// ---------------------------------------------------------------------------
// Next stop below: same as above, in the other direction
// ---------------------------------------------------------------------------
int Ko::nesteStoppUnder(int etasje) {
  int snuEtasje = -1;
  for (int i = etasje - 1; i >= 0; i--) {
    if (_bestiltInne[i] || _bestiltNed[i] || _bestiltEMERGENCY[i]) {
      return i;
    }
    if (erBestilt(i)) {
      snuEtasje = i;
    }
  }
  return snuEtasje;
}

// ---------------------------------------------------------------------------
// Should the cabin stop at this floor when travelling in this direction?
// Cabin calls and emergency stops always count, hall calls only in the
// same direction. Direction 0 counts every request.
// ---------------------------------------------------------------------------
bool Ko::skalBetjenes(int etasje, int retning) {
  if (retning == 1) {
    return _bestiltOpp[etasje] == true  || _bestiltInne[etasje] == true || _bestiltEMERGENCY[etasje] == true;
  } else if (retning == -1) {
    return _bestiltNed[etasje] == true || _bestiltInne[etasje] == true || _bestiltEMERGENCY[etasje] == true;
  } else {
    return _bestiltInne[etasje] == true  || _bestiltOpp[etasje] == true || _bestiltNed[etasje] == true || _bestiltEMERGENCY[etasje] == true;
  }
}

// ---------------------------------------------------------------------------
// Getters for the menu and the emergency mode
// ---------------------------------------------------------------------------
int Ko::valgtEtasje() {
  return _valgtEtasje;
}

int Ko::valgtRetning() {
  return _valgtRetning;
}

bool Ko::visMeny() {
  return _visMeny;
}

int Ko::count() {
  return _EMERGENCYcount;
}

bool Ko::EMERGENCY() {
  return _setEMERGENCY;
}
