//Define note names
typedef enum {
  C3, Cd3, D3, Dd3, E3, F3, Fd3, G3, Gd3, A3, Ad3, B3,
  C4, Cd4, D4, Dd4, E4, F4, Fd4, G4, Gd4, A4, Ad4, B4,
  C5, Cd5, D5, Dd5, E5, F5, Fd5, G5, Gd5, A5, Ad5, B5,
  C6, Cd6, D6, Dd6, E6, F6, Fd6, G6, Gd6, A6, Ad6, B6,
  C7, Cd7, D7, Dd7, E7, F7, Fd7, G7, Gd7, A7, Ad7, B7,
  C8, Cd8, D8, Dd8, E8, F8, Fd8, G8, Gd8, A8, Ad8, B8,
  SILENCE
} pitch;

const UWORD frequencies[] = { //values based on a formula used by the GB processor
    44,  156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986,
  1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
  1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
  1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
  1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
  1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015
};

//Define Instrument names
//Instruments should be confined to one channel
//due to different registers used between ch1, 2, 3, 4
typedef enum {
  NONE,
  MELODY,  //channel 2
  HARMONY //channel 3
} instrument;

// Define a note as having a pitch, instrument, and volume envelope
typedef struct {
	instrument i;
	pitch p;
	UBYTE env;
} note;

// Notes to be played on channel 2
note song_ch2[64] = { 
  {MELODY, A5, 0x81U},
  {MELODY, G5, 0x81U},
  {MELODY, C6, 0x81U},
  {MELODY, D6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {NONE, SILENCE, 0x00U},
  {MELODY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, A5, 0x81U},
  {MELODY, G5, 0x81U},
  {MELODY, D6, 0x81U},
  {MELODY, E6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, G6, 0x81U},
  {MELODY, A6, 0x81U},
  {MELODY, E6, 0x81U},

  {MELODY, D5, 0x81U},
  {MELODY, E5, 0x81U},
  {MELODY, G5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, A5, 0x81U},
  {MELODY, A5, 0x81U},
  {MELODY, E5, 0x81U},
  {MELODY, G5, 0x81U},
  {MELODY, A5, 0x81U},
  {MELODY, D6, 0x81U},
  {MELODY, A5, 0x81U},
  {MELODY, D6, 0x81U},
  {MELODY, E6, 0x81U},
  {MELODY, G6, 0x81U},
  {MELODY, A6, 0x81U},
  {NONE, SILENCE, 0x00U},

  {MELODY, C7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, E7, 0x81U},
  {MELODY, G7, 0x81U},
  {MELODY, D7, 0x81U},
  {MELODY, A6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, C7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, D7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, E7, 0x81U},
  {MELODY, A7, 0x81U},
  {MELODY, D7, 0x81U},
  {MELODY, A6, 0x81U},
  {MELODY, G6, 0x81U},

  {MELODY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {MELODY, C6, 0x81U},
  {MELODY, D6, 0x81U},
  {MELODY, G6, 0x81U},
  {MELODY, A6, 0x81U},
  {MELODY, G6, 0x81U},
  {MELODY, E6, 0x81U},
  {MELODY, D6, 0x81U},
  {MELODY, E6, 0x81U},
  {MELODY, A6, 0x81U},
  {MELODY, E6, 0x81U}
};

note song_ch3[64] = { 
  {HARMONY, A5, 0x81U},
  {HARMONY, G5, 0x81U},
  {HARMONY, C6, 0x81U},
  {HARMONY, D6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, A5, 0x81U},
  {HARMONY, G5, 0x81U},
  {HARMONY, D6, 0x81U},
  {HARMONY, E6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, G6, 0x81U},
  {HARMONY, A6, 0x81U},
  {HARMONY, E6, 0x81U},

  {HARMONY, D5, 0x81U},
  {HARMONY, E5, 0x81U},
  {HARMONY, G5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, A5, 0x81U},
  {HARMONY, A5, 0x81U},
  {HARMONY, E5, 0x81U},
  {HARMONY, G5, 0x81U},
  {HARMONY, A5, 0x81U},
  {HARMONY, D6, 0x81U},
  {HARMONY, A5, 0x81U},
  {HARMONY, D6, 0x81U},
  {HARMONY, E6, 0x81U},
  {HARMONY, G6, 0x81U},
  {HARMONY, A6, 0x81U},
  {NONE, SILENCE, 0x00U},

  {HARMONY, C7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, E7, 0x81U},
  {HARMONY, G7, 0x81U},
  {HARMONY, D7, 0x81U},
  {HARMONY, A6, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, C7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, D7, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, E7, 0x81U},
  {HARMONY, A7, 0x81U},
  {HARMONY, D7, 0x81U},
  {HARMONY, A6, 0x81U},
  {HARMONY, G6, 0x81U},

  {HARMONY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, A5, 0x81U},
  {NONE, SILENCE, 0x00U},
  {HARMONY, C6, 0x81U},
  {HARMONY, D6, 0x81U},
  {HARMONY, G6, 0x81U},
  {HARMONY, A6, 0x81U},
  {HARMONY, G6, 0x81U},
  {HARMONY, E6, 0x81U},
  {HARMONY, D6, 0x81U},
  {HARMONY, E6, 0x81U},
  {HARMONY, A6, 0x81U},
  {HARMONY, E6, 0x81U}
};

//function to set sound registers based on notes chosen
void set_note(note *n) {
  switch((*n).i) {
    case MELODY:
      NR21_REG = 0x42;
      NR22_REG = 0x75;
      NR23_REG = (UBYTE)frequencies[(*n).p]; //low bits of frequency
      NR24_REG = 0x80 | ((UWORD)frequencies[(*n).p]>>8); //high bits of frequency (and sound reset)
    break;
    case HARMONY:
      NR30_REG = 0x80;
      NR31_REG = 0x00;
      NR32_REG = 0x60;
      NR33_REG = (UBYTE)frequencies[(*n).p]; //low bits of frequency
      NR34_REG = 0x80 | ((UWORD)frequencies[(*n).p]>>8);
    break;
    case NONE:
    break;
	}
}

//This function plays whatever note is on
//the current beat in the channel
void play_channel2(INT8 current_beat) {
  set_note(&song_ch2[current_beat]);
}

void play_channel3(INT8 current_beat) {
  set_note(&song_ch3[current_beat]);
}