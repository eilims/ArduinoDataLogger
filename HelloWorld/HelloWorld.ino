uint8_t powerPin = 2;
uint8_t dataPin = A0;
uint8_t count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(powerPin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(powerPin, HIGH);
  delay(100);
  uint16_t data = analogRead(dataPin);
  //Time corresponding to measurement si based on CPU time not arduino
  //we can only send a byte. so we send if any additional computations are required.
  uint8_t divisions = data / 255;
  Serial.write(divisions);
  uint8_t send_data = data%255;
  Serial.write(send_data);
  delay(5000);
}
