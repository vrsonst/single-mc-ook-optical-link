void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
}

void loop() {
  digitalWrite(12, HIGH);
  delay(500);
  int readingOn = analogRead(A0);
  Serial.print("ON: ");
  Serial.println(readingOn);
  digitalWrite(12, LOW);
  delay(500);
  int readingOff = analogRead(A0);
  Serial.print("OFF: ");
  Serial.println(readingOff);
}