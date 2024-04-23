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
/*  SCREEN OUTPUT INFORMATION
* The SPI mode is the 4-wire mode, which is NOT the normal SCLK, MISO, MOSI, 
and SS signals of SPI, but are SCLK (labeled SCK), DC, MOSI (labeled SDA), and 
SS (labeled CS). That is, the display is write-only -- there is no output signal 
back to the micro driving it. The DC pin, when in SPI mode, is a Data/Command 
select input (Data when it's high or '1' and Command when it's low or '0'). Google
 the SSD1309 datasheet for details. And this is a separate output pin that you'll 
 need to supply from your micro that's outside of normal SPI operation.
* 

*Use U8g2 library for drivers
*Appropriate U8G2 constructor is https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#ssd1309-128x64_noname2
*   The above is confusing as hell and IDK what to do with it yet
*   Amazon review says that this works with SSD1306 as well.
*   How hard can it be??
* For multiple SPI connections, the chip select pin needs to be different, but supposedly they can all exist on the same bus.
*   I don't know why, but I read somewhere that the SD over SPI might cause issues. If that's the case then I think we might need to use the smaller OLED
* displays over I2C.
*
*/
//      INCLUDES

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <TMRpcm.h>
#include "Deck.h"
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "lambda_display.h"


//      INITIALIZATIONS
#define SPEAKER_PIN 9;
#define UNCONNECTED_ANALOG A0
#define SCORING_PIN A1
#define SD_ChipSelectPin 10
#define POT_PIN A2
#define PIE_SECTION_DEG 12
#define SCREEN_WIDTH 128
#define SCREEN_ChipSelectPin 8

const char zero_point_audio[] PROGMEM= "0pt.wav";
const char one_point_audio[] PROGMEM= "1pt.wav";
const char two_point_audio[] PROGMEM= "2pt.wav";
const char three_point_audio[] PROGMEM= "3pt.wav";
const char four_point_audio[] PROGMEM= "4pt.wav";

PGM_P const score_audio_table[] = {zero_point_audio, one_point_audio, two_point_audio, three_point_audio, four_point_audio};

//Line for loading strings from array into a string buffer.
//strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[i])));

char score_audio_buffer[7]; //check that this buffer is long enough. It could need an extra byte but idk.

Deck* deck;
char* left={};
char* right={};
char* Audio_file={};

const int button_pin = 2;
TMRpcm audio;

U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, SCREEN_ChipSelectPin);

byte scoring_wheel_deg=0;
byte pointer_deg = 0;
short int score_pointer_diff = 0;
byte score = 0;

void score_check(){
  //get hall sensor degree
  scoring_wheel_deg = ((byte)(analogRead(SCORING_PIN)-44)*(360.0/933));
  scoring_wheel_deg = scoring_wheel_deg%181;  
  //get potentiometer degree
  pointer_deg = (1023-analogRead(POT_PIN))*0.2933; //TODO Transformation of the value into the degree and rounded to the nearest degree. Should be in the range of a byte value.
  //check if pot degree is in scoring range (Assume center of 4 point slice)
  score_pointer_diff = abs(pointer_deg-scoring_wheel_deg);

  //determine where pot is in scoring range
  if(score_pointer_diff>PIE_SECTION_DEG/2+3*PIE_SECTION_DEG){
    score=0;
  }else{
    if(score_pointer_diff>PIE_SECTION_DEG/2+2*PIE_SECTION_DEG){
      score=1;
    }else{
      if(score_pointer_diff>PIE_SECTION_DEG/2+1*PIE_SECTION_DEG){
        score=2;
      }else{
        if(score_pointer_diff>PIE_SECTION_DEG/2){
          score=3;
        }else{
          if(score_pointer_diff<=PIE_SECTION_DEG/2){
            score=4; 
          }
        }
      }
    }
  }

  //Switch case for audio playback
  switch (score){
    case 0:
    strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
    break;

    case 1:
    strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
    break;

    case 2:
    strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
    break;

    case 3:
    strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
    break;

    case 4:
    strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
    break;
  }
  audio.play(score_audio_buffer);
}


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

  //Read scoring pin
  // delay(200);
  // Serial.println((analogRead(SCORING_PIN)-44)*(360.0/933));

  //Read potentiometer


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

  //display on screen
  u8g2.firstPage();
  do{

  draw_wrapped_text(left,0,u8g2);  
  draw_wrapped_text(right,SCREEN_WIDTH/2+3,u8g2);

  }while(u8g2.nextPage());

    //Audio file testing
    Audio_file = "womp.wav";

  audio.play(Audio_file);
  Serial.println(audio.isPlaying());
  while(audio.isPlaying());   //freeze the program while audio plays to avoid the memory leaks.
  // Serial.println("Audio played");  //SD card might need to be FAT16 instead of FAT32

    button_detector = false;  //This line is very important
  }
}
