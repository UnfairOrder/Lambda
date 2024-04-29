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

#include <Arduino.h>


//      INITIALIZATIONS
#define SPEAKER_PIN 5
#define UNCONNECTED_ANALOG A0

#define SD_ChipSelectPin 53

#define PIE_SECTION_DEG 15

//Should be 180 degrees in this space
#define POT_RIGHT_VAL 884
#define POT_LEFT_VAL 130
#define POT_PIN A2

//Scoring stuff
#define SCORING_OFFSET 180
#define SCORING_PIN A1


#define FONT u8g2_font_6x12_mf
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FONT_WIDTH 6
#define FONT_HEIGHT 12

#define DISPLAY_CS 6

#define DECK_SIZE 256   //if this changes, change the prime factors as well in the card modulus function
#define CARD_DIR "Cdt"

#define newCard_button_pin 2
#define reed_pin 3
#define replay_pin 4


// PGM_P const score_audio_table[] = {zero_point_audio, one_point_audio, two_point_audio, three_point_audio, four_point_audio};

//Line for loading strings from array into a string buffer.
//strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[i])));

char score_audio_buffer[13]; //check that this buffer is long enough. It could need an extra byte but idk.

File card_file;
char* left = {};
char* right={};
char* Audio_file={};

TMRpcm audio;





U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI u8g2(U8G2_R2, A4, A5, 6, 8, 7);


unsigned short scoring_wheel_deg=0;
short pointer_deg = 0;
short score_pointer_diff = 0;
byte score = 0;

void score_check(){
  //get hall sensor degree
  //Need to store in a larger variable first, then can modulous and cast to unsigned char.
  scoring_wheel_deg = ((unsigned short)(analogRead(SCORING_PIN)-SCORING_OFFSET)*(360.0/933));
  scoring_wheel_deg = scoring_wheel_deg%181;
  
  //get potentiometer degree
  // pointer_deg = analogRead(POT_PIN);

  pointer_deg = (unsigned short)(((analogRead(POT_PIN)-POT_LEFT_VAL)*(180.0/(POT_RIGHT_VAL-POT_LEFT_VAL))));
  if(pointer_deg>180){
    pointer_deg = 180;
  } 
  if(pointer_deg<0){
    pointer_deg =0;
  }
  // pointer_deg = pointer_deg-20;
  //check if pot degree is in scoring range (Assume center of 4 point slice)
  score_pointer_diff = abs(pointer_deg-scoring_wheel_deg);
  if(score_pointer_diff<0){
    score_pointer_diff = score_pointer_diff*-1;
  }


  //determine where pot is in scoring range
  //need to remove 1 pt. NO such thing exists in the game
  if(score_pointer_diff>PIE_SECTION_DEG/2+2*PIE_SECTION_DEG){
    score=0;
  }else{
    // if(score_pointer_diff>PIE_SECTION_DEG/2+2*PIE_SECTION_DEG){
    //   score=1;
    // }else{
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
  

  //Switch case for audio playback
  // switch (score){
  //   case 0:
  //   strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
  //   break;

  //   case 1:
  //   strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
  //   break;

  //   case 2:
  //   strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
  //   break;

  //   case 3:
  //   strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
  //   break;

  //   case 4:
  //   strcpy_P(score_audio_buffer, (PGM_P)pgm_read_word(&(score_audio_table[score])));
  //   break;

  //   default:
  //   Serial.println(F("SCORE ERROR"));
  //   break;
  // }

  sprintf(score_audio_buffer, "Audio/%ipt.wav",score);
  audio.play(score_audio_buffer);
  while(audio.isPlaying());

}
// const char* card_48_file = {"Cdt/48.txt"};
char filename[11];
char* file_buffer = nullptr;
unsigned short file_size;
void get_filename(const int &card,char* array){
  sprintf(array,"Cdt/%i",card);
  strcat(array,".txt");
}

short unsigned int shuffle=0;
byte deck_pos = 1;
short int drawn_card=0;

short unsigned int card_modulus(){
  randomSeed(analogRead(UNCONNECTED_ANALOG));
  random();
  short unsigned int random_num=5;
  do{
    random_num = (short unsigned)random(1,264);
  }while(random_num%2==0);
  return random_num;
}

bool newCard_button_detector = false;
void newCard_button_raise(){
  newCard_button_detector = true;
}


bool screen_open = false; //assign in setup.
void screen_reveal(){
  screen_open = true;
}
















                              //SETUP
void setup() {
  u8g2.begin();
  Serial.begin(9600);


  u8g2.firstPage();
  do{
    u8g2.setFont(FONT);
    u8g2.drawStr(2,20,"LAMBDA");
  }while(u8g2.nextPage());


  shuffle = card_modulus();



  pinMode(newCard_button_pin,INPUT_PULLUP);
  pinMode(reed_pin,INPUT_PULLUP);
  pinMode(replay_pin,INPUT);

  screen_open = !digitalRead(reed_pin);

//      INITIATE SERIAL CONNECTION



//        SEED RANDOM NUMBER
randomSeed(analogRead(UNCONNECTED_ANALOG));



// initialize the SD card
  // u8g2.clearDisplay();

if (!SD.begin(SD_ChipSelectPin)){
  u8g2.clearDisplay();
  u8g2.firstPage();
  Serial.println(F("SD_ERROR"));
  do{
    u8g2.setFont(FONT);
    u8g2.setCursor(2,20);
    u8g2.print(F("SD-Er"));

  }while(u8g2.nextPage());
  while(1);


}



//        SET SPEAKER PIN
  audio.speakerPin = SPEAKER_PIN;
  audio.quality(1);
  //setVolume(3) has minimal clipping on max volume.
  audio.setVolume(3);
  pinMode(SCORING_PIN, INPUT);


  u8g2.clearDisplay();
  u8g2.firstPage();
  do{
    u8g2.setCursor(2,20);
    u8g2.print(F("Draw A Card"));

  }while(u8g2.nextPage());

  
  Serial.println(F("BOOT COMPLETE"));
} //END SETUP








//For text Wrapping Code
byte i=1;
byte char_pos = 0;
byte offset = 0;
char substr_buf[11];




void loop() {

  //Read scoring pin
  // delay(200);
  // Serial.println((analogRead(SCORING_PIN)-44)*(360.0/933));

  //Read potentiometer
  //ATTACH INTERRUPT ONLY WORKS ON PINS 2 AND 3.
  //Reed sensor is finicky. Sometimes seems to do falling edge, sometimes doesn't.
  attachInterrupt(digitalPinToInterrupt(reed_pin), screen_reveal, FALLING);
  attachInterrupt(digitalPinToInterrupt(newCard_button_pin), newCard_button_raise, RISING);



  if(newCard_button_detector==true){
    if(file_buffer!=nullptr){
      delete[] file_buffer;
    }
    //do the card stuff
    //draw a card
    drawn_card = ((deck_pos*shuffle)%DECK_SIZE)+1;
    deck_pos+=1;
    Serial.println(deck_pos);


    //TESTING
    // drawn_card = 107;   //Least evil company card
    drawn_card = 48;

    if(drawn_card==48){ //This card is cursed
      drawn_card++;
    }

    //generate filename
    
    get_filename(drawn_card,filename);
    Serial.print(F("file opened: "));
    Serial.println(filename);
    //access file
    card_file = SD.open(filename);
    card_file.seek(0);  //go to beginning of file

  


    file_size = card_file.size();//card_file.size(); //this is how many characters long the file is.
    file_buffer = new char[file_size];

    //read into buffer
    for(int i=0; i<file_size+1; i++){
      file_buffer[i] = card_file.read();
    }
    card_file.close();
 
    // Serial.println(file_buffer);

    left = strtok(file_buffer,"\n");
    right = strtok(NULL,"\n");
    Audio_file = strtok(NULL,"\n");
    Audio_file[strlen(Audio_file)-1] = '\0';
    strtok(NULL,"\n");



  Serial.println(left);
  Serial.println(right);
  Serial.println(Audio_file);


  //display on u8g2
  /*
  * TODO: I don't think that the lambda_display function works as is. Test in a new doc to ensure functionality before importing.
  */
  u8g2.clearDisplay();
  u8g2.firstPage();
  do{

    u8g2.drawVLine(SCREEN_WIDTH/2,0,64);
    u8g2.drawVLine(SCREEN_WIDTH/2-1,0,64);

    // draw_wrapped_text(left,0,u8g2);

    //Draw Left Text
    u8g2.setFont(FONT);
    i=1;
    char_pos = 0;
    offset = 0;

    


    //does the full lines
      while (i<=(strlen(left)/9)){
        //split the string at the nearest space
        offset=0;
        while(left[char_pos+9-offset]!=' '&& offset<9){                  //THIS CODE NEEDS TO BE FIXED. ADD SOMETHING HERE TO ACCOUNT FOR WORDS LONGER THAN THE LINE LENGTH. UNDERRATED IS AN EXAMPLE THAT IS CAUSING AN ISSUE.
          offset++;

        }
        if(offset>=9){
          offset = 2;
          strncpy(substr_buf,left+(char_pos),10-offset);
          substr_buf[10-offset]='-';
          substr_buf[10-offset+1]='\0';
        }else{
          
          strncpy(substr_buf,left+(char_pos),10-offset);
          substr_buf[10-offset]='\0';
        }
        // Serial.println(offset);
        u8g2.drawStr(3, i*FONT_HEIGHT+2, substr_buf);
        i++;
        char_pos+=10;
        char_pos-=offset;
      }
    //finish up
      i--;
      strncpy(substr_buf,left+(char_pos),strlen(left)-(char_pos));
      substr_buf[strlen(left)-(char_pos)]='\0';
      u8g2.drawStr(3, (i+1)*FONT_HEIGHT+2, substr_buf);  
    // draw_wrapped_text(right,SCREEN_WIDTH/2+3,u8g2);
    
    
    
    
    //right side code
    i=1;
    char_pos = 0;
    offset = 0;


    //does the full lines
      while (i<=(strlen(left)/9)){
        //split the string at the nearest space
        offset=0;
        while(right[char_pos+9-offset]!=' '&&offset<9){
          offset++;
        }
        if(offset>=9){
          offset = 2;
          strncpy(substr_buf,right+(char_pos),10-offset);
          substr_buf[10-offset]='-';
          substr_buf[10-offset+1]='\0';
        }else{
          strncpy(substr_buf,right+(char_pos),10-offset);
          substr_buf[10-offset]='\0';
        }
        u8g2.drawStr(SCREEN_WIDTH/2+3, i*FONT_HEIGHT+2, substr_buf);
        i++;
        char_pos+=10;
        char_pos-=offset;
      }
    //finish up
      i--;
      strncpy(substr_buf,right+(char_pos),strlen(right)-(char_pos));
      substr_buf[strlen(right)-(char_pos)]='\0';
      u8g2.drawStr(SCREEN_WIDTH/2+3, (i+1)*FONT_HEIGHT+2, substr_buf);

  // // Serial.println(F("SC_DR"));

  }while(u8g2.nextPage());


  audio.play(Audio_file);
  if(audio.isPlaying()){

  }
  while(audio.isPlaying());   //freeze the program while audio plays to avoid the memory leaks.
  // Serial.println("Audio played");  //SD card might need to be FAT16 instead of FAT32

    newCard_button_detector = false;  //This line is very important

  }

  if(digitalRead(replay_pin)==HIGH){
    audio.play(Audio_file);
    if(audio.isPlaying()){
      // Serial.println(F("Audio replayed"));
    }
    while(audio.isPlaying());
    while(digitalRead(replay_pin)==HIGH){
      delay(100);
    }
  }

  if(screen_open){
    // Serial.println(F("SCREEN OPEN"));
    score_check();
    // score_check();
    delay(500);
    screen_open=false;
  }



  
}
