
#ifndef DECK_H
#define DECK_H

#include<Arduino.h>

class Deck{
  public:
  Deck(short unsigned int q);
  short unsigned int draw_card();

  private:
  short unsigned int card_list[265];
  short unsigned int card_pos;

};

#endif