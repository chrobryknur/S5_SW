#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define BAUD 9600 
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define DOT_DELAY 500
#define DASH_DELAY 1500

#define ELEMENT_DELAY 500
#define CHAR_DELAY 1500
#define WORD_DELAY 3500

const char *alpha[] = {
    ".-",   //A
    "-...", //B
    "-.-.", //C
    "-..",  //D
    ".",    //E
    "..-.", //F
    "--.",  //G
    "....", //H
    "..",   //I
    ".---", //J
    "-.-",  //K
    ".-..", //L
    "--",   //M
    "-.",   //N
    "---",  //O
    ".--.", //P
    "--.-", //Q
    ".-.",  //R
    "...",  //S
    "-",    //T
    "..-",  //U
    "...-", //V
    ".--",  //W
    "-..-", //X
    "-.--", //Y
    "--..", //Z
};

void uart_init()
{
    UBRR0 = UBRR_VALUE;
    UCSR0A = 0;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream)
{
    while(!(UCSR0A & _BV(UDRE0)));
    UDR0 = data;
    return 0;
}

int uart_receive(FILE *stream)
{
    while (!(UCSR0A & _BV(RXC0)));
    return UDR0;
}

FILE uart_file;

int main()
{
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    DDRD = _BV(PD2) | _BV(PD3);

    char input[128];

    while (1)
    {
        scanf("%s", input);

        for(uint16_t i=0; input[i] != 0; i++)
        {
            int letter_index = input[i] - 'A';

            for(uint16_t j = 0; alpha[letter_index][j] != 0; j++)
            {
                PORTD = _BV(PD2);
                _delay_ms(alpha[letter_index][j] == '-' ? DASH_DELAY : DOT_DELAY);

                PORTD = 0;
                _delay_ms(ELEMENT_DELAY);
            }
            input[0] = '\0';
        }
        PORTD = _BV(PD3);
        _delay_ms(CHAR_DELAY);
        PORTD = 0;
    }
}

