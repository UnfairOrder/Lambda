#include "Deck.h"

Deck::Deck(short unsigned int q){
  for(short unsigned i=1; i<266; i++){
    card_list[i-1] = ((i*q)%265)+1;
  }
  card_pos = 0;
}

short unsigned int Deck::draw_card(){
  short unsigned int card = card_list[card_pos];
  card_pos = (card_pos+1)%265;
  return card;
}