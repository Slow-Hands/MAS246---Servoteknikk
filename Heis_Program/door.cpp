//door.cpp

#include <Arduino.h>

#include "door.h"
#include "dac.h"

//Fase tabell for steppermotor.
const int phaseTable[4][2] = {
  {0, 0},  // steg 1
  {0, 1},  // steg 2
  {1, 1},  // steg 3
  {1, 0}   // steg 4
};

Door::Door() {
    state = DoorState::CLOSED;
    phase = 0; // row 0 of the phase table, matches begin()
    position = 0;
    target = 0;
    lastStep = 0;
}

void Door::begin() {
    pinMode(PIN_A_ENABLE, OUTPUT);
    pinMode(PIN_B_PHASE, OUTPUT);
    pinMode(PIN_B_ENABLE, OUTPUT); 
    pinMode(PIN_A_PHASE, OUTPUT);

    digitalWrite(PIN_A_ENABLE, HIGH);
    digitalWrite(PIN_B_PHASE, LOW);
    digitalWrite(PIN_B_ENABLE, HIGH); 
    digitalWrite(PIN_A_PHASE, LOW);

    dac_init();
    set_dac(4095, 4095);

    position = OPEN_STEPS;
    state = DoorState::OPEN;
    closeDoor();
}

void Door::openDoor(){
    target = OPEN_STEPS;
}

void Door::closeDoor(){
    target = CLOSE_STEPS; 
}

void Door::update() {
  if (position == target) {
    if (target == OPEN_STEPS) {
      state = DoorState::OPEN;
    } else {
      state = DoorState::CLOSED;
    }
    return;
  }

  if (micros() - lastStep < STEP_INTERVAL) {
    return;
  }

  if (target > position) {
    stepOnce(1);
  } else {
    stepOnce(-1);
  }

  lastStep = micros();
  state = DoorState::HALF;
}

bool Door::isClosed(){
    return state == DoorState::CLOSED;    
}

bool Door::isOpen() {
    return state == DoorState::OPEN;
}

void Door::stepOnce(int dir) {
  phase = (phase + dir + 4) % 4;   // holder phase mellom 0 og 3, begge veier

  digitalWrite(PIN_A_PHASE, phaseTable[phase][0]);
  digitalWrite(PIN_B_PHASE, phaseTable[phase][1]);

  position += dir;
}