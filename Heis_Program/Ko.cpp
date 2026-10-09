#include "Ko.h"

static const int KNAPP_PINS[ANTALL_ETASJER] = { 22, 23, 24, 25, 26, 27, 28, 29 };
static const int LED_PINS[ANTALL_ETASJER]   = { 49, 48, 47, 46, 45, 44, 43, 42 };

Ko::Ko() {
  _maalEtasje = 0;
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    _forrigeKnapp[i] = false;
    _ledPaa[i] = false;
  }
}

void Ko::begin() {
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
    pinMode(KNAPP_PINS[i], INPUT_PULLUP);
  }
}

void Ko::oppdater() {
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    bool knappNaa = digitalRead(KNAPP_PINS[i]);

    if (knappNaa == LOW && _forrigeKnapp[i] == HIGH) {
      for (int j = 0; j < ANTALL_ETASJER; j++) {
        digitalWrite(LED_PINS[j], LOW);
        _ledPaa[j] = false;
      }

      _ledPaa[i] = true;
      _maalEtasje = i;
      digitalWrite(LED_PINS[i], HIGH);
    }

    _forrigeKnapp[i] = knappNaa;
  }
}

int Ko::maalEtasje() {
  return _maalEtasje;
}