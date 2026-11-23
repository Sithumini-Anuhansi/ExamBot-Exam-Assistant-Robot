// Define Pins
const int trigPin = 13;
const int echoPin = 12;

// Define variables
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  Serial.begin(115200);     // Starting Serial Communication
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, return the sound wave travel time in microseconds
  // The timeout (30000) prevents the code from freezing if no object is detected
  duration = pulseIn(echoPin, HIGH, 30000);

  // Calculate the distance
  // Speed of sound is ~0.034 cm/us. We divide by 2 (there and back)
  distance = duration * 0.034 / 2;

  // Print the distance on the Serial Monitor
  if (distance == 0) {
    Serial.println("Out of range / No object detected");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500); // Wait half a second before next reading
}