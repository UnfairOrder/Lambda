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

#include <SD.h>
#include <SPI.h>
#define SD_ChipSelectPin 10  // SD select is on pin 04
#include <TMRpcm.h>
#define SPEAKER_PIN 9;
#include "Deck.h"

#define UNCONNECTED_ANALOG A0
#define SCORING_PIN A1

//      INITIALIZATIONS



TMRpcm audio;

void get_filename(const int &card,char* array){
  sprintf(array,"%i",card);
  strcat(array,".txt");
}


short unsigned int card_modulus(){
  short unsigned int random_num=5;
  do{
    random_num = (short unsigned)random(1,264);
  }while(random_num==5 || random_num == 53);
  return random_num;
}

bool button_detector = false;
void button_raise(){
  button_detector = true;
}


//                        GLOBAL DECLARATIONS



Deck* deck;
char* left={};
char* right={};
char* Audio_file={};


const int button_pin = 2;



                              //SETUP
void setup() {

  pinMode(button_pin,INPUT_PULLUP);


  //      INITIATE SERIAL CONNECTION
Serial.begin(9600);

Serial.println(F("Boot complete"));
//        SEED RANDOM NUMBER
randomSeed(analogRead(UNCONNECTED_ANALOG));

//        BUILD DECK
deck = new Deck(card_modulus());


// initialize the SD card
if (!SD.begin(SD_ChipSelectPin)){
  Serial.println(F("Initialization failed"));
  while(1);
}
Serial.println(F("Initialization done"));

//        SET SPEAKER PIN
  audio.speakerPin = SPEAKER_PIN;
  pinMode(SCORING_PIN, INPUT);
}

void loop() {

  delay(200);
  Serial.println((analogRead(SCORING_PIN)-44)*(360.0/933));

  attachInterrupt(digitalPinToInterrupt(button_pin), button_raise, RISING);
  if(button_detector==true){
    //do the card stuff
    //draw a card
    short int drawn_card = deck->draw_card();
    //generate filename
    char filename[7];
    get_filename(drawn_card,filename);
    //access file
    File card_file = SD.open(filename);
    card_file.seek(0);  //go to beginning of file
    // Serial.print("file opened: ");
    // Serial.print(filename);
    Serial.print("card: ");
    Serial.println(drawn_card);

    unsigned long file_size = card_file.size();//card_file.size(); //this is how many characters long the file is.
    char file_buffer[file_size];

    //read into buffer
    for(int i=0; i<file_size+1; i++){
      file_buffer[i] = card_file.read();
    }
    card_file.close();
 
    // Serial.println(file_buffer);

    left = strtok(file_buffer,"\n");
    right = strtok(NULL,"\n");
    Audio_file = strtok(NULL,"\n");
    strtok(NULL,"\n");

    Serial.println(left);
    Serial.println(right);
    Serial.println(Audio_file);

    //Audio file testing
    Audio_file = "womp-womp.wav";

  audio.play(Audio_file);
  Serial.println("Audio played");  //SD card might need to be FAT16 instead of FAT32

    button_detector = false;  //This line is very important
  }

  

}
