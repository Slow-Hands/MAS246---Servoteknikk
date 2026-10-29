// door.ino
// Antar stepper motor er døren.
// Krav til stadier for dørene (OPEN, CLOSED, HALF).
// Ved oppstart antar vi dørene er åpne og lukker dørene.

#include "door.h"

Door door;

bool lastOpen = false;
bool lastClosed = false;

void setup() {
  Serial.begin(9600);
  door.begin();
}

void loop() {
  door.update();

  bool nowOpen = door.isOpen();
  bool nowClosed = door.isClosed();

  // Skriv bare ut når tilstanden endrer seg
  if (nowOpen != lastOpen || nowClosed != lastClosed) {
    lastOpen = nowOpen;
    lastClosed = nowClosed;

    if (nowOpen) {
      Serial.println("OPEN");
    } else if (nowClosed) {
      Serial.println("CLOSED");
    } else {
      Serial.println("HALF");
    }
  }
}