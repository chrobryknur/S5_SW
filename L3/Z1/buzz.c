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
    quareter_note,
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
    20
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
}

static const sound melody[] PROGMEM = {
    {C,  quarter_note},
    {C,  quarter_note},
    {G,  quarter_note},
    {G,  quarter_note},
    {A,  quarter_note},
    {A,  quarter_note},
    {G,  quarter_note},
    {F,  quarter_note},
    {F,  quarter_note},
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
