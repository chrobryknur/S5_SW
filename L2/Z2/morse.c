#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define BAUD 9600 
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define BTN PD2
#define BTN_PIN PIND

#define LED PD3
#define LED_DDR DDRD

#define BUFFER_SIZE 8

#define CYCLE_DELAY 10
#define CYCLES_FOR_DOT 10
#define CYCLES_FOR_DASH 100
#define CYCLES_FOR_CHAR_TRANSMISSION_END 200

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

void print_character(char* buffer){
    for(int i=0;i<26;i++){
        if(!strcmp(buffer, alpha[i])){
            putchar((char)('A'+i));
            return;
        }
    }
}

int main() {
    LED_DDR |= _BV(LED);
    PORTD   |= _BV(BTN);

    FILE uart_file;
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    char buffer[BUFFER_SIZE];
    char* ptr = buffer;
    int8_t  is_recording_in_progress = 0;
    int16_t button_pressed_since = 0;
    int16_t button_not_pressed_since = 0;

    while (1) {
        if(BTN_PIN & _BV(BTN)){
            PORTD &= ~_BV(LED);
            if(button_pressed_since > CYCLES_FOR_DOT){
                if(button_pressed_since > CYCLES_FOR_DASH){
                    *ptr = '-';
                }
                else{
                    *ptr = '.';
                }
                ptr++;
                button_pressed_since = 0;
            }

            button_not_pressed_since++;
            if(is_recording_in_progress && button_not_pressed_since > CYCLES_FOR_CHAR_TRANSMISSION_END){
                *ptr = '\0';
                print_character(buffer);
                ptr = buffer;
                is_recording_in_progress = 0;
            }
        }
        else{
            is_recording_in_progress = 1;
            button_not_pressed_since = 0;
            button_pressed_since++;
            if(button_pressed_since > CYCLES_FOR_DASH){
                PORTD |= _BV(LED);
            }
        }                
        _delay_ms(CYCLE_DELAY);
    }
}
