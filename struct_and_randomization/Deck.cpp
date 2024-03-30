#include "Deck.h"

Deck::Deck(int q){
  for(size_t i=1; i<266; i++){
    card_list[i-1] = ((i*q)%265)+1;
  }
  card_pos = 0;
}

int Deck::draw_card(){
  int card = card_list[card_pos];
  card_pos = (card_pos+1)%265;
  return card;
}