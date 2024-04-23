#define pot_pin A3

static const float pot_conversion = 300/1023;
int pot_val=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  pot_val = analogRead(pot_pin);
  Serial.println((1023-pot_val)*0.2933);
  delay(200);
}
