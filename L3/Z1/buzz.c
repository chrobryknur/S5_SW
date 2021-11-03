#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>

#define BUZZ PB5
#define BUZZ_DDR DDRB
#define BUZZ_PORT PORTB

typedef enum note{
    whole_note = 0,
    half_note,
    quarter_note,
    eighth_note,
    sixteenth_note,
    thirty_second_note
} note;

static const uint16_t note_lengths[] = {
    1600,
    800,
    400,
    200,
    100,
    50,
    25
}; 

typedef enum tone{
    C = 0,
    D,
    E,
    F,
    G,
    A,
    H,
    CC,
    pause
} tone;

static const uint16_t tone_frequencies[] = {
    523,
    587,
    659,
    698,
    783,
    880,
    987,
    1046,
    10
};

typedef struct sound{
    uint8_t t;
    uint8_t n;
} sound;

void delay_us(unsigned long us)
{
    while (us--) _delay_us(1);
}

void play_sound(sound s){
    uint32_t step  = tone_frequencies[s.t];
    uint32_t delay = note_lengths[s.n];

    for (uint32_t i = 0; i < ((uint32_t)1000 * (delay) / (step)) / 2; i++) {
      BUZZ_PORT |= _BV(BUZZ);
      delay_us(step);
      BUZZ_PORT &= ~_BV(BUZZ);
      delay_us(step);
    }
    _delay_ms(10);
}

static const sound melody[] PROGMEM = {
    {C,  whole_note},
    {D,  whole_note},
    {E,  whole_note},
    {F,  whole_note},
    {G,  whole_note},
    {A,  whole_note},
    {H,  whole_note},
    {CC,  whole_note},
    {pause,  whole_note},
    {C,  half_note},
    {D,  half_note},
    {E,  half_note},
    {F,  half_note},
    {G,  half_note},
    {A,  half_note},
    {H,  half_note},
    {CC,  half_note},
    {pause,  half_note},
    {C,  quarter_note},
    {D,  quarter_note},
    {E,  quarter_note},
    {F,  quarter_note},
    {G,  quarter_note},
    {A,  quarter_note},
    {H,  quarter_note},
    {CC,  quarter_note},
    {pause,  quarter_note},
    {C,  eighth_note},
    {D,  eighth_note},
    {E,  eighth_note},
    {F,  eighth_note},
    {G,  eighth_note},
    {A,  eighth_note},
    {H,  eighth_note},
    {CC,  eighth_note},
    {pause,  eighth_note},
    {C,  sixteenth_note},
    {D,  sixteenth_note},
    {E,  sixteenth_note},
    {F,  sixteenth_note},
    {G,  sixteenth_note},
    {A,  sixteenth_note},
    {H,  sixteenth_note},
    {CC,  sixteenth_note},
    {pause,  sixteenth_note},
    {C,  thirty_second_note},
    {D,  thirty_second_note},
    {E,  thirty_second_note},
    {F,  thirty_second_note},
    {G,  thirty_second_note},
    {A,  thirty_second_note},
    {H,  thirty_second_note},
    {CC,  thirty_second_note},
    {pause,  thirty_second_note},
    {C,  sixteenth_note},
    {D,  sixteenth_note},
    {E,  sixteenth_note},
    {F,  sixteenth_note},
    {G,  sixteenth_note},
    {A,  sixteenth_note},
    {H,  sixteenth_note},
    {CC,  sixteenth_note},
    {pause,  sixteenth_note},
    {C,  eighth_note},
    {D,  eighth_note},
    {E,  eighth_note},
    {F,  eighth_note},
    {G,  eighth_note},
    {A,  eighth_note},
    {H,  eighth_note},
    {CC,  eighth_note},
    {pause,  eighth_note},
    {C,  quarter_note},
    {D,  quarter_note},
    {E,  quarter_note},
    {F,  quarter_note},
    {G,  quarter_note},
    {A,  quarter_note},
    {H,  quarter_note},
    {CC,  quarter_note},
    {pause,  quarter_note},
    {C,  half_note},
    {D,  half_note},
    {E,  half_note},
    {F,  half_note},
    {G,  half_note},
    {A,  half_note},
    {H,  half_note},
    {CC,  half_note},
    {pause,  half_note},
    {C,  whole_note},
    {D,  whole_note},
    {E,  whole_note},
    {F,  whole_note},
    {G,  whole_note},
    {A,  whole_note},
    {H,  whole_note},
    {CC,  whole_note},
    {pause,  whole_note},
};

union read
{
    uint16_t ui;
    sound s;
};

int main() {
    BUZZ_DDR |= _BV(BUZZ);
    while (1) {
        for(uint16_t i = 0; i < sizeof(melody); i++){
            union read r;
            r.ui = pgm_read_word(&melody[i]);
            play_sound(r.s);
        }
    }
}
