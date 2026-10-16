#include "Ko.h"

static const int KNAPP_PINS[ANTALL_ETASJER] = { 22, 23, 24, 25, 26, 27, 28, 29 };
static const int LED_PINS[ANTALL_ETASJER] = { 49, 48, 47, 46, 45, 44, 43, 42 };

Ko::Ko() {
  _maalEtasje = 0;
  for (int i = 0; i < ANTALL_ETASJER; i++) {
    _forrigeKnapp[i] = false;
    _bestilt[i] = false;
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

      _bestilt[i] = true;
      _maalEtasje = i;
      digitalWrite(LED_PINS[i], HIGH);
    }

    _forrigeKnapp[i] = knappNaa;
  }
}

int Ko::maalEtasje() {
  return _maalEtasje;
}

bool Ko::erBestilt(int etasje) {
  return _bestilt[etasje];
}

void Ko::fjernBestilling(int etasje) {
  _bestilt[etasje] = false;
  digitalWrite(LED_PINS[etasje], LOW);                
}

int Ko::nesteOver(int etasje) {                    
  for (int i = etasje + 1; i < ANTALL_ETASJER; i++) {  
    if (_bestilt[i]) {                                
      return i;
    }
  }
  return -1;
}

int Ko::nesteUnder(int etasje) {                    
  for (int i = etasje - 1; i >= 0; i--) {  
    if (_bestilt[i]) {                                
      return i;
    }
  }
  return -1;
}