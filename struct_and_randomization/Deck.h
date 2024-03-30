
#ifndef DECK_H
#define DECK_H

#include<Arduino.h>

class Deck{
  public:
  Deck(int q);
  int draw_card();

  private:
  int card_list[265];
  int card_pos;

};

#endif