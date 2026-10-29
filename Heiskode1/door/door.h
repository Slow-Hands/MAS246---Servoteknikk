//door.h

#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>

enum class DoorState {OPEN, CLOSED, HALF};
class Door {
    public:
    Door();
    void begin(); 
    void openDoor();
    void closeDoor();
    void update();
    bool isClosed();
    bool isOpen();

    private: 
    DoorState state;
    void stepOnce(int dir);
    static const int PIN_A_ENABLE = 69;
    static const int PIN_A_PHASE = 68;
    static const int PIN_B_ENABLE = 67;
    static const int PIN_B_PHASE = 66; 

    static const long OPEN_STEPS = 200;
    static const long CLOSE_STEPS = 0;
    static const unsigned long STEP_INTERVAL = 10000; //microseconds between steps

    int phase; //row in the phase table, 0-3
    long position; //current step count, 0 = closed
    long target; // step count the door is changing to
    unsigned long lastStep; //micros() at the last step.

};


#endif