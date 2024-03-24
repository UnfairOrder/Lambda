// Audio file playing
/*
* file type must be .wav
* bit resolution 8-bit
* sampling rate 16000 Hz
* audio channels to mono
* Set PCM format as PCM unsigned 8-bit

https://maker.pro/arduino/projects/arduino-audio-player
https://www.instructables.com/Playing-Wave-file-using-arduino/
-install tmr pcm library from : https://github.com/TMRh20/TMRpcm/wiki
--This can also be done with the library manager in the ide.

*/
//      INCLUDES

// #include <SD.h>
#define SD_ChipSelectPin 4  // SD select is on pin 04
// #include <TMRpcm.h>
#define SPEAKER_PIN 9;
#include "card_list.h"


//      INITIALIZATIONS

// TMRpcm tmrpcm;





int i=0;

void setup() {
Serial.begin(9600);
Serial.println(Card_List[0].left);
Serial.println(Card_List[264].right);

}

void loop() {
  if (i<265){
    Serial.print(Card_List[i].left);
    Serial.print(", ");
    Serial.print(Card_List[i].right);
    Serial.print(", ");
    Serial.print(Card_List[i].Audio_file);
    Serial.println();
    i+=1;
  }


}
