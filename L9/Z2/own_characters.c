#include <avr/io.h>
#include <avr/pgmspace.h>
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

void lcd_init()
{
  LCD_Initialize();
  LCD_Clear();
  LCD_GoTo(0, 1);
}

const char progress_char_0[8] PROGMEM = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const char progress_char_1[8] PROGMEM = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
const char progress_char_2[8] PROGMEM = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
const char progress_char_3[8] PROGMEM = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
const char progress_char_4[8] PROGMEM = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
const char progress_char_5[8] PROGMEM = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

void define_char(uint8_t addr, const char* custom_char)
{
  LCD_WriteCommand(HD44780_CGRAM_SET | addr*8);
  for(uint8_t i = 0; i < 8; i++)
    LCD_WriteData(pgm_read_byte(&custom_char[i]));
}

void add_chars()
{
  define_char(0, progress_char_0);
  define_char(1, progress_char_1);
  define_char(2, progress_char_2);
  define_char(3, progress_char_3);
  define_char(4, progress_char_4);
  define_char(5, progress_char_5);
}

void draw_progress_bar(uint8_t progress)
{
  LCD_GoTo(0, 0);
  printf("%.2d%%", (progress * 100) / 80);

  LCD_GoTo(0, 1);

  while(progress > 5){
    putchar(5);
    progress -= 5;
  }

  putchar(progress % 6);
}

int main()
{
  lcd_init();
  fdev_setup_stream(&hd44780_file, hd44780_transmit, NULL, _FDEV_SETUP_WRITE);
  stdout = stderr = &hd44780_file;

  add_chars();
  
  while(1) {
    for(uint8_t i = 0; i <= 80; i++){
      draw_progress_bar(i);
      _delay_ms(500);
    }
    LCD_Clear();
  }
}
