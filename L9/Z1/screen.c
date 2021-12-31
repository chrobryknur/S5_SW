#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include "hd44780.h"

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem
#define CHARACTERS_PER_LINE 16

int hd44780_transmit(char data, FILE *stream)
{
  LCD_WriteData(data);
  return 0;
}

FILE hd44780_file;
FILE uart_file;

void uart_init(){
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_receive(FILE *stream){
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

uint8_t lower_line_content[CHARACTERS_PER_LINE];

void shift_display_up(uint8_t* characters){
  LCD_Clear();
  LCD_GoTo(0, 0);
  for(uint8_t i = 0; i < *characters; i++){
    putchar(lower_line_content[i]);
    _delay_ms(2);
  }
  LCD_GoTo(0, 1);
  *characters = 0;
}

void lcd_init()
{
  LCD_Initialize();
  LCD_Clear();
  LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_NOBLINK);
  LCD_GoTo(0, 1);
}

int main()
{
  lcd_init();
  fdev_setup_stream(&hd44780_file, hd44780_transmit, NULL, _FDEV_SETUP_WRITE);
  stdout = stderr = &hd44780_file;

  uart_init();
  fdev_setup_stream(&uart_file, NULL, uart_receive, _FDEV_SETUP_READ);
  stdin = &uart_file;

  uint8_t cursor_position = 0;
  
  while(1) {
    uint8_t data = getchar();
  
    if(data == '\r'){
      shift_display_up(&cursor_position);
      continue;
    }

    lower_line_content[cursor_position++] = data;

    putchar(data);
    _delay_ms(2);

    if(cursor_position == CHARACTERS_PER_LINE){
      shift_display_up(&cursor_position);
    }
  }
}
