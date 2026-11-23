// --- PIN DEFINITIONS ---
const int ENA = 14; const int IN1 = 25; const int IN2 = 26; // Left Motor
const int ENB = 27; const int IN3 = 33; const int IN4 = 32; // Right Motor

const int SENSOR_L = 34; // GPIO 34
const int SENSOR_C = 35; // GPIO 35
const int SENSOR_R = 36; // GPIO 36

// --- CALIBRATION ---
const int threshold = 2000; // Increase if it stops on white, decrease if it misses black
const int baseSpeed = 160;   
const int turnSpeed = 140;

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  Serial.begin(115200);
  
  Serial.println("Robot Initialized. Starting in 2 seconds...");
  delay(2000); 
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, baseSpeed); analogWrite(ENB, baseSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, turnSpeed); analogWrite(ENB, turnSpeed);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, turnSpeed); analogWrite(ENB, turnSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);    analogWrite(ENB, 0);
}

void loop() {
  int L = analogRead(SENSOR_L);
  int C = analogRead(SENSOR_C);
  int R = analogRead(SENSOR_R);

  // DETECT T-JUNCTION OR START BOX (All sensors see black)
  if (L > threshold && C > threshold && R > threshold) {
    Serial.println("T-Junction Detected! Stopping for 10s...");
    stopMotors();
    delay(10000); // 10 Second Stop
    
    // Move forward slightly to clear the junction so it doesn't trigger again immediately
    moveForward();
    delay(300); 
  }
  // NORMAL LINE FOLLOWING
  else if (C > threshold) {
    moveForward();
  } 
  else if (L > threshold) {
    turnLeft();
  } 
  else if (R > threshold) {
    turnRight();
  } 
  // LOST THE LINE
  else {
    stopMotors(); 
  }
  
  delay(10); 
}