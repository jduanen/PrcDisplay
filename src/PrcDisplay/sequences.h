/***************************************************************************
*
* PrcDisplay EL Wire Pattern Sequences
* 
***************************************************************************/


#define NUM_SEQUENCES     8

#define MAX_SEQUENCES     8
#define MAX_PATTERNS      32

// A Pattern is a set of active HIGH enables and a modifier to the base dwell time
// The duration that a Pattern is displayed is given by the number of cycles in
//  base value given by the Pattern's baseDelay value (multipled by DELAY_INTERVAL msecs)
typedef struct {
  unsigned char enables;  // active HIGH for each channel, LSB=channelA, MSB=channelH
  short baseDelay;        // number of cycles to add/subtract from the base dwell time
} Pattern;

// A Sequence is an array of (pattern, dwell modifier) tuples
// Can have as many Sequences as there are selector switch positions
// A NULL pointer will make the given selection generate a random pattern
// N.B. counting on initialization putting zeros in the remaining slots
Pattern sequences[MAX_SEQUENCES][MAX_PATTERNS] = {
  {             // Sequence #0
    {0, 0},
  },
  {             // Sequence #1
    {0b00000000, 0},
    {0b11111111, 0},
  },
  {             // Sequence #2
    {0b00000000, 0},
    {0b00000001, 0},
    {0b00000011, 0},
    {0b00000111, 0},
    {0b00001111, 0},
    {0b00011111, 0},
    {0b00111111, 0},
    {0b01111111, 0},
    {0x11111111, 0},
    {0b01111111, 0},
    {0b00111111, 0},
    {0b00011111, 0},
    {0b00001111, 0},
    {0b00000111, 0},
    {0b00000011, 0},
    {0b00000001, 0},
  },
  {             // Sequence #3
    {0b00000000, 0},
    {0b00000001, 0},
    {0b00000010, 0},
    {0b00000100, 0},
    {0b00001000, 0},
    {0b00010000, 0},
    {0b00100000, 0},
    {0b01000000, 0},
    {0b10000000, 0},
    {0b01000000, 0},
    {0b00100000, 0},
    {0b00010000, 0},
    {0b00001000, 0},
    {0b00000100, 0},
    {0b00000010, 0},
    {0b00000001, 0},
    {0b10000000, 0},
    {0b01000000, 0},
    {0b00100000, 0},
    {0b00010000, 0},
    {0b00001000, 0},
    {0b00000100, 0},
    {0b00000010, 0},
    {0b00000001, 0},
    {0b00000010, 0},
    {0b00000100, 0},
    {0b00001000, 0},
    {0b00010000, 0},
    {0b00100000, 0},
    {0b01000000, 0},
    {0b10000000, 0},
  },
  {             // Sequence #4
    {0x00000000, 0},
    {0b10000001, 0},
    {0b11000011, 0},
    {0b11000011, 0},
    {0b11100111, 0},
    {0b11111111, 0},
    {0b11100111, 0},
    {0b11000011, 0},
    {0b10000001, 0},
    {0x00000000, 0},
    {0b10000001, 0},
    {0b01000010, 0},
    {0b00100100, 0},
    {0b00011000, 0},
    {0b00100100, 0},
    {0b01000010, 0},
    {0b10000001, 0},
    {0b11111111, 0},
    {0b01111110, 0},
    {0b00111100, 0},
    {0b00011000, 0},
    {0b00111100, 0},
    {0b01111110, 0},
    {0b11111111, 0},
  },
  {             // Sequence #5
    {0b10101010, 0},
    {0b01010101, 0},
  },
  {             // Sequence #6
    {0b11001100, 0},
    {0b01100110, 0},
    {0b00110011, 0},
    {0b10011001, 0},
  },
  {             // Sequence #7
    {0b11111111, 0},
    {0b00000000, 0},
    {0b11111111, 0},
    {0b00000000, 0},
    {0b11111111, 0},
    {0b00000000, 0},
    {0b11111111, 0},
    {0b00000000, 0},
    {0b11110000, 0},
    {0b00001111, 0},
    {0b11110000, 0},
    {0b00001111, 0},
    {0b11110000, 0},
    {0b00001111, 0},
    {0b11110000, 0},
    {0b00001111, 0},
    {0b11001100, 0},
    {0b00110011, 0},
    {0b11001100, 0},
    {0b00110011, 0},
    {0b11001100, 0},
    {0b00110011, 0},
    {0b11001100, 0},
    {0b00110011, 0},
    {0b01010101, 0},
    {0b10101010, 0},
    {0b01010101, 0},
    {0b10101010, 0},
    {0b01010101, 0},
    {0b10101010, 0},
    {0b01010101, 0},
    {0b10101010, 0},
  }
};
