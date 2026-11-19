// =============================================================================
// Heis_Program.ino - main program for the MAS246 elevator
//
// Connects all modules and runs the control loop. Every 20 ms it reads the
// position, updates the state machine, generates the S-curve reference,
// updates the LCD, runs the PID controller and sends data on the serial port.
// =============================================================================

#include "Posisjon.h"
#include "Display.h"
#include "Regulator.h"
#include "Heis.h"
#include "Profil.h"
#include "door.h"
#include "Vekt.h"

// ---------------------------------------------------------------------------
// Global objects: one of each module.
// Regulator: direction pin 6, PWM pin 7. Profil: v_max = 1500 counts/s,
// a_max = 2000 counts/s^2. Vekt: potentiometer on A0, overload at 3300 kg.
// ---------------------------------------------------------------------------
Display display;
Regulator regulator(6, 7);
Heis heis;
Door door;
Profil profil(1500, 2000);
Vekt vekt(A0, 3300);

unsigned long forrigeTid = 0;   // time of the last control sample [ms]

// ---------------------------------------------------------------------------
// Variables for logging trips to heislogg.py
// ---------------------------------------------------------------------------
bool forrigeFerdig = true;          // was the reference finished in the last sample?
unsigned long turStart = 0;         // start time of the current trip [ms]
int etterlogg = 0;                  // samples left to log after the trip has ended
const int ETTERLOGG_SAMPLER = 100;  // log 100 samples (2 s) after each trip

// ---------------------------------------------------------------------------
// setup(): start the serial port and initialise all modules
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  posisjonBegin();
  profil.start(lesPosisjon());
  regulator.begin();
  display.begin();
  heis.begin();
  door.begin();


  forrigeTid = millis();
}

// ---------------------------------------------------------------------------
// loop(): one pass of the program
// ---------------------------------------------------------------------------
void loop() {
  // The door stepper is updated in every pass, since it needs a step every
  // 10 ms. The rest of the loop only runs once every 20 ms.
  door.update();
  unsigned long naa = millis();
  if (naa - forrigeTid < 20) return;
  float dt = (naa - forrigeTid) / 1000.0;
  forrigeTid = naa;

  // Read the position and find the nearest floor
  long pos = lesPosisjon();
  int etasjeNaa = naermesteEtasje(pos);

  // Update the state machine (queue, door and target floor)
  heis.oppdater(etasjeNaa, pos, door, vekt.overvekt());

  // Generate the S-curve reference towards the target floor
  long maalPos = etasjePosisjon(heis.maalEtasje());
  long avstand = maalPos - pos;

  long ref = profil.oppdater(maalPos, dt);
  bool ferdig = (ref == maalPos);
  long folgefeil = ref - pos;

  // Direction arrow for the LCD: 1 = up, -1 = down, 0 = at the target
  int pil;
  if (avstand > TOLERANSE) {
    pil = 1;
  } else if (avstand < -TOLERANSE) {
    pil = -1;
  } else {
    pil = 0;
  }


  // Update the LCD: emergency text, order menu or normal floor view
  if (heis.EMERGENCY()) {
    display.EMERGENCY();
  } else {
    if (heis.visMeny()) {
      display.visMeny(heis.valgtEtasje(), heis.valgtRetning());
    } else {
      display.vis(etasjeNaa, pil);
      display.visVekt(vekt.kg(), vekt.overvekt());
    }

  }

  // PID controller sets the motor output from the tracking error
  regulator.oppdater(folgefeil, pos, dt, ferdig);

  // Debug output every sample: reference, position, error, speed,
  // emergency encoder count and state
  Serial.print(ref);
  Serial.print(" ");
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(folgefeil);
  Serial.print(" ");
  Serial.print(regulator.fart());
  Serial.print(" ");
  Serial.print(heis.count());
  Serial.print(" ");
  Serial.println(heis.tilstand());


  // ---------- Logging for heislogg.py ----------
  // A trip starts when the profile goes from finished to not finished
  // (new target). A header line tells heislogg.py to start a new file.
  if (!ferdig && forrigeFerdig) {
    turStart = naa;
    Serial.println("t_ms,ref,pos,v");
  }

  if (!ferdig) {
    etterlogg = ETTERLOGG_SAMPLER;  // keep the log open while the trip is running
  }

  // One CSV line per sample: time since start, reference, position, speed
  if (!ferdig || etterlogg > 0) {
    Serial.print(naa - turStart);  Serial.print(',');
    Serial.print(ref);             Serial.print(',');
    Serial.print(pos);             Serial.print(',');
    Serial.println(regulator.fart());
    if (ferdig) etterlogg--;        // count down the extra samples after the trip
  }

  forrigeFerdig = ferdig;
}
