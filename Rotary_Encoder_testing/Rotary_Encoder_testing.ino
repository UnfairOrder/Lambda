#define outputA 6
#define outputB 7

int pot_pin = A3;
int pot_val = 0;



int counter=0;
int aLastState;
int aState;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);

  Serial.begin(9600);

  aLastState = digitalRead(outputA);
}

void loop() {
  aState = digitalRead(outputA);
  if(aState != aLastState){
    if (digitalRead(outputB)!=aState){
      counter++;
    }
    else{
      counter--;
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;

  // put your main code here, to run repeatedly:
  // This does the potentiometer delay and light change stuff.
  // pot_val = analogRead(pot_pin);
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(pot_val);
  // digitalWrite(LED_BUILTIN,LOW);
  // delay(pot_val);



}
