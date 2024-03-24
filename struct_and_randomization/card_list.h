
#ifndef CARD_LIST_H
#define CARD_LIST_H



//      CARD STRUCT

struct card{
  const char *left;
  const char *right;
  const char *Audio_file;
  //card(const char* _left, const char* _right, const char* _Audio_file);
};

const int list_length = 267;

// need to add length here
const card Card_List[]={
  {"Good", "Bad", "Goodbad.wav"},
  {"Mildly addictive","Highly addictive","addictive.wav"}
};

#endif