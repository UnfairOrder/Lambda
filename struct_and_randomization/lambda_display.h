#ifndef LAMBDA_DISPLAY_H
#define LAMBDA_DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define FONT u8g2_font_6x12_mf
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FONT_WIDTH 6
#define FONT_HEIGHT 12

void draw_wrapped_text(const char* str_in, const byte &x_off, const U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI &screen);

#endif