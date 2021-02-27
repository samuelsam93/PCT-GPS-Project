#define DONEPIN PIN_A5

void setup() {
  // put your setup code here, to run once:
  delay(10000);
  pinMode(DONEPIN, OUTPUT);
  digitalWrite(DONEPIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
}
