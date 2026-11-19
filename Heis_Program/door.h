// =============================================================================
// door.h - elevator door driven by a stepper motor
//
// The door is opened and closed in steps without blocking the rest of the
// program. Its state is OPEN, HALF (moving) or CLOSED.
// =============================================================================

#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>

enum class DoorState {OPEN, CLOSED, HALF};
class Door {
    public:
    Door();
    void begin();
    void openDoor();     // set the target to open
    void closeDoor();    // set the target to closed
    void update();       // take one step towards the target if it is time
    bool isClosed();
    bool isOpen();

    private:
    DoorState state;
    void stepOnce(int dir);

    // Pins of the DRV8813 stepper driver
    static const int PIN_A_ENABLE = 69;
    static const int PIN_A_PHASE = 68;
    static const int PIN_B_ENABLE = 67;
    static const int PIN_B_PHASE = 66;

    static const long OPEN_STEPS = 100;                  // steps from closed to open
    static const long CLOSE_STEPS = 0;
    static const unsigned long STEP_INTERVAL = 10000;    // microseconds between steps

    int phase;                // row in the phase table, 0-3
    long position;            // current step count, 0 = closed
    long target;              // step count the door is moving to
    unsigned long lastStep;   // micros() at the last step

};


#endif
