const int irPin = 39;
int sensorMin = 4095;
int sensorMax = 0;

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);
  Serial.println("Move paper back and forth to calibrate...");
}

void loop() {
  int val = analogRead(irPin);

  // Track the range of values seen
  if (val < sensorMin) sensorMin = val;
  if (val > sensorMax) sensorMax = val;

  Serial.print("Current: "); Serial.print(val);
  Serial.print(" | Min: "); Serial.print(sensorMin);
  Serial.print(" | Max: "); Serial.print(sensorMax);
  Serial.print(" | Suggested Threshold: "); Serial.println((sensorMin + sensorMax) / 2);

  delay(5000);
}