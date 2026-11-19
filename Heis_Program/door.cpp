// =============================================================================
// door.cpp - elevator door driven by a stepper motor
// =============================================================================

#include <Arduino.h>

#include "door.h"
#include "dac.h"

// ---------------------------------------------------------------------------
// Phase table for the stepper motor in full-step mode.
// Each row gives the phase of winding A and B for one step.
// ---------------------------------------------------------------------------
const int phaseTable[4][2] = {
  {0, 0},  // step 1
  {0, 1},  // step 2
  {1, 1},  // step 3
  {1, 0}   // step 4
};

// ---------------------------------------------------------------------------
// Constructor: door closed, no steps taken
// ---------------------------------------------------------------------------
Door::Door() {
    state = DoorState::CLOSED;
    phase = 0; // row 0 of the phase table, matches begin()
    position = 0;
    target = 0;
    lastStep = 0;
}

// ---------------------------------------------------------------------------
// Setup: enable both windings, set the winding current with the DAC, and
// close the door. The door is assumed to be open at power-up.
// ---------------------------------------------------------------------------
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
    set_dac(4095, 4095);   // full current reference on both windings

    position = OPEN_STEPS;
    state = DoorState::OPEN;
    closeDoor();
}

// ---------------------------------------------------------------------------
// Commands: only set the target, the movement happens in update()
// ---------------------------------------------------------------------------
void Door::openDoor(){
    target = OPEN_STEPS;
}

void Door::closeDoor(){
    target = CLOSE_STEPS;
}

// ---------------------------------------------------------------------------
// Called in every pass of loop(). At the target the state becomes OPEN or
// CLOSED. Otherwise one step is taken every STEP_INTERVAL, and the state
// is HALF while the door moves.
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// State queries
// ---------------------------------------------------------------------------
bool Door::isClosed(){
    return state == DoorState::CLOSED;
}

bool Door::isOpen() {
    return state == DoorState::OPEN;
}

// ---------------------------------------------------------------------------
// One step in direction dir (+1 = open, -1 = close): go to the next row of
// the phase table and write it to the driver
// ---------------------------------------------------------------------------
void Door::stepOnce(int dir) {
  phase = (phase + dir + 4) % 4;   // keeps phase between 0 and 3 in both directions

  digitalWrite(PIN_A_PHASE, phaseTable[phase][0]);
  digitalWrite(PIN_B_PHASE, phaseTable[phase][1]);

  position += dir;
}
