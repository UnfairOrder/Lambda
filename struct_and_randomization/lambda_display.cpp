#include "lambda_display.h"


void draw_wrapped_text(const char* str_in, const byte &x_off, const U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI &screen){
  byte i=1;
  byte char_pos = 0;
  byte offset = 0;
  char substr_buf[11];

  //does the full lines
    while (i<=(strlen(str_in)/10)){
      //split the string at the nearest space
      offset=0;
      while(str_in[char_pos+9-offset]!=' '){
        offset++;
      }
      strncpy(substr_buf,str_in+(char_pos),10-offset);
      substr_buf[10-offset]='\0';
      screen.drawStr(x_off, i*FONT_HEIGHT+2, substr_buf);
      i++;
      char_pos+=10;
      char_pos-=offset;
    }
  //finish up
    i--;
    strncpy(substr_buf,str_in+(char_pos),strlen(str_in)-(char_pos));
    substr_buf[strlen(str_in)-(char_pos)]='\0';
    screen.drawStr(x_off, (i+1)*FONT_HEIGHT+2, substr_buf);
}