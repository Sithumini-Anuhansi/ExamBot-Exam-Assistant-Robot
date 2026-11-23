// Left Motor Pins
const int ENA = 14; 
const int IN1 = 25;
const int IN2 = 26;

// Right Motor Pins
const int ENB = 27;
const int IN3 = 33;
const int IN4 = 32;

// Set a low speed (Value between 0 and 255)
// Note: Most motors won't move below 70-80 due to friction
int lowSpeed = 70; 

void setup() {
  Serial.begin(115200);

  // Set all pins as Outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Testing Motors: Moving forward at low speed...");
}

void loop() {
  // --- Move Left Motor Forward (FIXED) ---
  digitalWrite(IN1, HIGH);   // Changed from HIGH
  digitalWrite(IN2, LOW);  // Changed from LOW
  analogWrite(ENA, lowSpeed);

  // --- Move Right Motor Forward (STAYS SAME) ---
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, lowSpeed);

  delay(2000); // Run for 2 seconds

  // --- Stop All ---
  Serial.println("Stopping motors...");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  delay(5000); // Wait 5 seconds before repeating
}