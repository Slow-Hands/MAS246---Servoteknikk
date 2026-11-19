// =============================================================================
// dac.cpp - driver for the MCP4922 12-bit DAC (two channels)
//
// The serial interface is bit-banged directly through the port registers of
// port C (pins 30-33), which is much faster than digitalWrite().
// =============================================================================

#include "dac.h"

// ---------------------------------------------------------------------------
// Pin macros: set or clear one bit of PORTC
// ---------------------------------------------------------------------------
#define LDACPIN_LOW  ( PORTC &= B01111111 ) // PC7 = D30
#define LDACPIN_HIGH ( PORTC |= B10000000 )
#define SDIPIN_LOW   ( PORTC &= B10111111 ) // PC6 = D31
#define SDIPIN_HIGH  ( PORTC |= B01000000 )
#define SCKPIN_LOW   ( PORTC &= B11011111 ) // PC5 = D32
#define SCKPIN_HIGH  ( PORTC |= B00100000 )
#define CSPIN_LOW    ( PORTC &= B11101111 ) // PC4 = D33
#define CSPIN_HIGH   ( PORTC |= B00010000 )

// Clock one bit into the DAC, and send a 1 or a 0
#define CLOCK        SCKPIN_HIGH; SCKPIN_LOW;
#define SET_HIGH     SDIPIN_HIGH; CLOCK;
#define SET_LOW      SDIPIN_LOW;  CLOCK;


// ---------------------------------------------------------------------------
// Set port C pins 4-7 as outputs, set the idle levels and both outputs to 0
// ---------------------------------------------------------------------------
void dac_init(void)
{
	
DDRC |= B11110000;
  CSPIN_HIGH;
  SCKPIN_LOW;
  SDIPIN_LOW;
  LDACPIN_HIGH;
  set_dac (0,0);	
	
}



// ---------------------------------------------------------------------------
// Write value_a to output A and value_b to output B.
// Each output gets a 16-bit word: 4 configuration bits (15-12) followed by
// the 12 data bits (11-0), most significant bit first. LDAC then updates
// both outputs at the same time.
// ---------------------------------------------------------------------------
void set_dac ( int value_a, int value_b )
{

  // --- output A ---  (bit 15 = 0 selects channel A, 13 = gain 1x, 12 = active)
  CSPIN_LOW;

  SDIPIN_HIGH; CLOCK; // 15
  SDIPIN_LOW;  CLOCK; // 14
  SDIPIN_HIGH; CLOCK; // 13
  CLOCK;              // 12
  
  SDIPIN_LOW;  if ( 2048 & value_a ){ SDIPIN_HIGH; } CLOCK; // 11
  SDIPIN_LOW;  if ( 1024 & value_a ){ SDIPIN_HIGH; } CLOCK; // 10
  SDIPIN_LOW;  if (  512 & value_a ){ SDIPIN_HIGH; } CLOCK; // 09
  SDIPIN_LOW;  if (  256 & value_a ){ SDIPIN_HIGH; } CLOCK; // 08
  SDIPIN_LOW;  if (  128 & value_a ){ SDIPIN_HIGH; } CLOCK; // 07
  SDIPIN_LOW;  if (   64 & value_a ){ SDIPIN_HIGH; } CLOCK; // 06
  SDIPIN_LOW;  if (   32 & value_a ){ SDIPIN_HIGH; } CLOCK; // 05
  SDIPIN_LOW;  if (   16 & value_a ){ SDIPIN_HIGH; } CLOCK; // 04
  SDIPIN_LOW;  if (    8 & value_a ){ SDIPIN_HIGH; } CLOCK; // 03
  SDIPIN_LOW;  if (    4 & value_a ){ SDIPIN_HIGH; } CLOCK; // 02
  SDIPIN_LOW;  if (    2 & value_a ){ SDIPIN_HIGH; } CLOCK; // 01
  SDIPIN_LOW;  if (    1 & value_a ){ SDIPIN_HIGH; } CLOCK; // 00

  CSPIN_HIGH; 
  

  // --- output B ---  (bit 15 = 1 selects channel B)
  CSPIN_LOW;

  SDIPIN_LOW; CLOCK;  // 15
  CLOCK;              // 14
  SDIPIN_HIGH; CLOCK; // 13
  CLOCK;              // 12

  SDIPIN_LOW;  if ( 2048 & value_b ){ SDIPIN_HIGH; } CLOCK; // 11
  SDIPIN_LOW;  if ( 1024 & value_b ){ SDIPIN_HIGH; } CLOCK; // 10
  SDIPIN_LOW;  if (  512 & value_b ){ SDIPIN_HIGH; } CLOCK; // 09
  SDIPIN_LOW;  if (  256 & value_b ){ SDIPIN_HIGH; } CLOCK; // 08
  SDIPIN_LOW;  if (  128 & value_b ){ SDIPIN_HIGH; } CLOCK; // 07
  SDIPIN_LOW;  if (   64 & value_b ){ SDIPIN_HIGH; } CLOCK; // 06
  SDIPIN_LOW;  if (   32 & value_b ){ SDIPIN_HIGH; } CLOCK; // 05
  SDIPIN_LOW;  if (   16 & value_b ){ SDIPIN_HIGH; } CLOCK; // 04
  SDIPIN_LOW;  if (    8 & value_b ){ SDIPIN_HIGH; } CLOCK; // 03
  SDIPIN_LOW;  if (    4 & value_b ){ SDIPIN_HIGH; } CLOCK; // 02
  SDIPIN_LOW;  if (    2 & value_b ){ SDIPIN_HIGH; } CLOCK; // 01
  SDIPIN_LOW;  if (    1 & value_b ){ SDIPIN_HIGH; } CLOCK; // 00

  CSPIN_HIGH;

  // Pulse LDAC to update both outputs
  LDACPIN_LOW;
  SCKPIN_HIGH;
  LDACPIN_HIGH;
  SCKPIN_LOW;

}