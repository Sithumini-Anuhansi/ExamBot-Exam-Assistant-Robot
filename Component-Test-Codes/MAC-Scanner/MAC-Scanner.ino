#include "WiFi.h"

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Station mode is required to get the MAC
  
  // Wait for serial to initialize
  delay(1000);
  
  Serial.println();
  Serial.print("ESP32 MAC Address: "); //ESP32 MAC Address: xx:xx:xx:xx:xx:xx
  Serial.println(WiFi.macAddress());
}
 
void loop(){
  // Nothing to do here
}