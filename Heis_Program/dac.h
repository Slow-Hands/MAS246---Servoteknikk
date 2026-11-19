// =============================================================================
// dac.h - driver for the MCP4922 12-bit DAC (two channels)
//
// Sets the current reference of the two stepper windings of the door.
// =============================================================================

#include "Arduino.h"

void dac_init(void);                        // set up the pins and set both outputs to 0

void set_dac (int value_a, int value_b);    // write 0-4095 to output A and B
