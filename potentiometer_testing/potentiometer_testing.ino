#define pot_pin A3
int pot_val=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  pot_val = analogRead(pot_pin);
  Serial.println(pot_val);

}
