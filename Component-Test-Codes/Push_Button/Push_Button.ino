#include <esp_now.h> // Include the ESP-NOW library for low-power wireless communication
#include <WiFi.h>    // Include WiFi library as ESP-NOW requires WiFi hardware to be active

// PIN DEFINITION
#define BUTTON_PIN 15 // Define GPIO 15 as the input pin for the physical button

// MAC address must use 0x and commas
// This is the unique hardware address of the receiving ESP32 (the robot)
uint8_t robotAddress[] = {0x28, 0x05, 0xA5, 0x07, 0xDF, 0x88}; 

// Define a data structure to hold the message we want to send
typedef struct struct_message {
    int tableID;        // Stores the ID of the table calling the robot
    bool requestActive; // A flag to tell the robot to start moving
} struct_message;

struct_message myData; // Create an instance of the struct called 'myData'

// This callback function runs automatically after a message is sent to check if it arrived
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  // Print "Success" if the receiver acknowledged the packet, otherwise "Fail"
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200); // Initialize Serial Monitor at 115200 baud rate
  
  // Set the button pin as input with an internal pull-up resistor (stays HIGH until pressed)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA); // Set WiFi to Station mode (required for ESP-NOW)

  // Initialize the ESP-NOW protocol
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return; // Exit setup if initialization fails
  }

  // Register the "OnDataSent" function to be called whenever a message is transmitted
  esp_now_register_send_cb(OnDataSent);
  
  // Define peer information (details about the receiver)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, robotAddress, 6); // Copy the robot's MAC address into peerInfo
  peerInfo.channel = 0;   // Use the current WiFi channel
  peerInfo.encrypt = false; // Disable encryption for faster, simpler communication
  
  // Register the robot as a "peer" so this ESP32 knows where to send data
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return; // Exit setup if peer registration fails
  }
}

void loop() {
  // If button is pressed, the pin is connected to Ground, making the reading LOW
  if (digitalRead(BUTTON_PIN) == LOW) {
    myData.tableID = 2;        // Set the table ID to 2
    myData.requestActive = true; // Set the activation flag to true
    
    // Send the 'myData' structure to the 'robotAddress'
    esp_err_t result = esp_now_send(robotAddress, (uint8_t *) &myData, sizeof(myData));
    
    // Print whether the ESP32 successfully handed off the data to the WiFi radio
    Serial.print("Send Result: ");
    Serial.println(result == ESP_OK ? "Sent" : "Send Error");
    
    delay(2000); // Wait 2 seconds (debounce) to prevent a single press from sending multiple times
  }
}