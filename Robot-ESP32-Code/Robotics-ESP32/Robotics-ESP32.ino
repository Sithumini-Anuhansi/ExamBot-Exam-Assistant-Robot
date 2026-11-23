#include <Arduino.h>   // Core Arduino library for ESP32
#include <SPI.h>       // Serial Peripheral Interface for RFID communication
#include <MFRC522.h>   // Library for the MFRC522 RFID reader
#include <esp_now.h>   // ESP-NOW protocol for wireless communication
#include <WiFi.h>      // Required for ESP32 WiFi/ESP-NOW features

// ================= PIN DEFINITIONS =================
#define ENA 14         // PWM pin for Left Motor speed control
#define IN1 25         
#define IN2 26         
#define ENB 27         // PWM pin for Right Motor speed control
#define IN3 33         
#define IN4 32         

#define SS_PIN     5   // SDA (Slave Select) pin
#define RST_PIN    22  // Reset pin

#define TRIG       13  // outputs pulse
#define ECHO       12  // receives pulse

#define SENSOR_L   34  // Left IR Line Sensor (Analog input)
#define SENSOR_C   35  // Center IR Line Sensor (Analog input)
#define SENSOR_R   36  // Right IR Line Sensor (Analog input)

#define TRAY_IR    39  // IR Sensor for paper detection (Analog input)

// ================= SETTINGS & CONSTANTS =================
const int THRESHOLD      = 2000;  // Analog value above which the sensor "sees" black
const int TRAY_THRESHOLD = 2500;  // Threshold to determine if paper is present
const int SPEED          = 140;   // General driving speed (0-255)
const int TURN_SPD       = 160;   // Turning speed
const int DIST_STOP      = 15;    // Stop distance if obstacle is detected (cm)

bool systemActive = false;        // Flag to track if the robot is currently moving
bool paperPresent = true;         // Flag to track the paper's presence on the tray
unsigned long lastDebugTime = 0;  // Timer for serial print intervals
unsigned long lastRFIDTime  = 0;  // Timer to prevent repeat RFID scans

MFRC522 rfid(SS_PIN, RST_PIN);     // Create MFRC522 instance

// Data structure for ESP-NOW messages
typedef struct struct_message {
    int tableID;
    bool requestActive;
} struct_message;
struct_message incomingData;

// ================= COMMUNICATION CALLBACK =================
// runs automatically whenever a signal is received from another ESP32
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incoming, int len) {
  memcpy(&incomingData, incoming, sizeof(incomingData)); // Copy received data to our structure
  if(incomingData.requestActive) {                        // Check if the signal is an "Active" request
    systemActive = true;                                  // Wake up the robot
    Serial.printf(">>> [ESP-NOW] Signal Received! Target Table: %d\n", incomingData.tableID);
  }
}

// ================= MOTOR CONTROL =================
void drive(int lSpd, int rSpd) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);  // Set Left Motor direction forward
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);  // Set Right Motor direction forward
  ledcWrite(ENA, lSpd); ledcWrite(ENB, rSpd);        // Apply PWM speed to both motors
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);    // Stop power to motors
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);    
  ledcWrite(ENA, 0);    ledcWrite(ENB, 0);           // Set PWM speed to zero
}

// ================= SENSOR FUNCTIONS =================
/*Ultrasonic Sensor*/
long getDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);     // Ensure Trigger is clear
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);   // Send 10-microsecond sonic burst
  digitalWrite(TRIG, LOW);                           // Turn off Trigger
  long duration = pulseIn(ECHO, HIGH, 25000);        // Measure time for echo to return
  return (duration == 0) ? 999 : (duration * 0.034 / 2); // Convert time to centimeters
}

/*RFID Module*/
void scanRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return; // Exit if no card found
  if (millis() - lastRFIDTime < 3000) return;         // Skip if we scanned this tag very recently
  
  String tagID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {          // Loop through tag UID bytes
    tagID += String(rfid.uid.uidByte[i], HEX);        // Convert bytes to Hex string
  }
  Serial.print(">>> [RFID SCAN] Location Tag: "); Serial.println(tagID); // Log location
  lastRFIDTime = millis();                            // Update the last scan timer
}

/*IR Sensor for Paper Detection*/
void checkTrayStatus() {
  int trayVal = analogRead(TRAY_IR);                  // Read infrared reflection from tray
  
  if (paperPresent && trayVal < TRAY_THRESHOLD) {     // If paper was there, but reflection dropped
    Serial.println(">>> [TRAY] Paper REMOVED from tray!"); // Print event
    paperPresent = false;                             // Update status to "Removed"
  } 
  else if (!paperPresent && trayVal > TRAY_THRESHOLD) { // If paper was gone, but reflection increased
    Serial.println(">>> [TRAY] Paper PLACED on tray."); // Print event
    paperPresent = true;                              // Update status to "Present"
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);                               // Start Serial communication
  WiFi.mode(WIFI_STA);                                // Initialize WiFi in Station mode
  
  if (esp_now_init() == ESP_OK) {                     // Start ESP-NOW wireless protocol
    esp_now_register_recv_cb(OnDataRecv);             // Link the callback function for data reception
  }

  SPI.begin();                                        // Start SPI bus for RFID
  rfid.PCD_Init();                                    // Initialize RFID hardware

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);         // Set motor direction pins as outputs
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);         // Set motor direction pins as outputs
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);        // Set Ultrasonic pins
  
  pinMode(SENSOR_L, INPUT);                           // Set Left Line Sensor as input
  pinMode(SENSOR_C, INPUT);                           // Set Center Line Sensor as input
  pinMode(SENSOR_R, INPUT);                           // Set Right Line Sensor as input
  pinMode(TRAY_IR, INPUT);                            // Set Tray Sensor as input

  ledcAttach(ENA, 5000, 8);                           // Attach PWM to ENA (5kHz, 8-bit)
  ledcAttach(ENB, 5000, 8);                           // Attach PWM to ENB (5kHz, 8-bit)
}

// ================= MAIN LOOP =================
void loop() {
  int valL = analogRead(SENSOR_L);                    // Read analog value from Left sensor
  int valC = analogRead(SENSOR_C);                    // Read analog value from Center sensor
  int valR = analogRead(SENSOR_R);                    // Read analog value from Right sensor
  long dist = getDistance();                          // Get latest obstacle distance

  bool L = (valL > THRESHOLD);                        // Determine if Left is on black line
  bool C = (valC > THRESHOLD);                        // Determine if Center is on black line
  bool R = (valR > THRESHOLD);                        // Determine if Right is on black line

  checkTrayStatus();                                  // Constantly check if paper is taken
  scanRFID();                                         // Constantly check for location tags

  if (millis() - lastDebugTime > 400) {               // Debug print every 400ms
    Serial.printf("L:%d C:%d R:%d | Dist: %ldcm | Tray: %d\n", valL, valC, valR, dist, analogRead(TRAY_IR));
    lastDebugTime = millis();
  }

  if (dist < DIST_STOP) {                             // If obstacle is detected
    stopMotors();                                     // Emergency stop
    return;                                           // Exit loop early to prevent movement
  }

  if (!systemActive) {                                // If robot is waiting for orders
    if (C && !L && !R) { systemActive = true; }       // Start if placed on a single center line
    else { stopMotors(); return; }                    // Otherwise stay still
  }

  // NAVIGATION & JUNCTION LOGIC
  if (L && C && R) {                                  // T-JUNCTION: All 3 sensors see black
    Serial.println(">>> [EVENT] T-Junction! Waiting 10s...");
    stopMotors();                                     // Stop for delivery
    for(int i = 0; i < 100; i++) {                    // Split 10s into small chunks
      checkTrayStatus();                              // Check tray while waiting
      delay(100);                                     // 100ms * 100 = 10 seconds total
    }
    drive(SPEED, SPEED);                              // Resume driving
    delay(500);                                       // Drive past the junction line to avoid re-triggering
  } 
  else if (L) {                                       // DEVIATED LEFT: Robot is too far right
    drive(0, TURN_SPD);                               // Slow left motor to pivot back onto line
  } 
  else if (R) {                                       // DEVIATED RIGHT: Robot is too far left
    drive(TURN_SPD, 0);                               // Slow right motor to pivot back onto line
  } 
  else if (C) {                                       // ON TRACK: Center sensor is on line
    drive(SPEED, SPEED);                              // Drive straight
  } 
  else {                                              // LOST LINE: All sensors see white
    stopMotors();                                     // Stop to prevent wandering
    systemActive = false;                             // Enter standby mode
  }

  delay(10);                                          // Tiny delay for system stability
}