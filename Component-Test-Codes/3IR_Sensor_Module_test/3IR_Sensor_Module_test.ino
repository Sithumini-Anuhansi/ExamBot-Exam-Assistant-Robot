// Define Pins
const int sensorLeft = 34;   
const int sensorCenter = 35; 
const int sensorRight = 36;  

// Setting a manual threshold - Adjust this based on your testing
const int THRESHOLD = 1800; 

void setup() {
  Serial.begin(115200);
  
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  Serial.println("--- IR Threshold Calibration Mode ---");
}

void loop() {
  displaySensorData();
  delay(6000); // Delay for readability
}

// Method to display raw values and compare them to the threshold
void displaySensorData() {
  // Reading raw analog values (0 to 4095)
  int rawL = analogRead(sensorLeft);
  int rawC = analogRead(sensorCenter);
  int rawR = analogRead(sensorRight);

  Serial.println("---------------------------------------");
  Serial.print("RAW VALUES ->  L: "); Serial.print(rawL);
  Serial.print(" | C: "); Serial.print(rawC);
  Serial.print(" | R: "); Serial.println(rawR);
  
  Serial.print("THRESHOLD  ->  "); Serial.println(THRESHOLD);

  // Logic to show what the robot "sees" based on the threshold
  Serial.print("DETECTION  ->  ");
  Serial.print(rawL < THRESHOLD ? "[BLACK] " : "[WHITE] ");
  Serial.print(rawC < THRESHOLD ? "[BLACK] " : "[WHITE] ");
  Serial.print(rawR < THRESHOLD ? "[BLACK]  " : "[WHITE]  ");
  Serial.println();
}