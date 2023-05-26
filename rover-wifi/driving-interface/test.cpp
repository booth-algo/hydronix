#include <Arduino.h>
#include "DrivingSystem.h"
//global variables:
Driving_system buggy(26,25,33,32);

void setup() {
  Serial.begin(115200);
  Serial.println("setup started");
  buggy.begin();
  
  Serial.println("setup complete");
}



void loop() {
  Serial.print("hall: "); Serial.println(hallRead());
  buggy.set(255,0);
  Serial.println("walking");
  delay(2000);
  buggy.set(-255,0);
  Serial.println("walking backward");
  delay(2000);
  buggy.set(0,-225);
  Serial.println("left");
  delay(2000);
  buggy.set(0,255);
  Serial.println("right");
  delay(2000);
  buggy.set(255,-255);
  Serial.println("fwd left");
  delay(2000);
  buggy.set(255,255);
  Serial.println("fwd right");
  delay(2000);
  buggy.set(-255,-255);
  Serial.println("bwd left");
  delay(2000);
  buggy.set(-255,255);
  Serial.println("bwd right");
  delay(2000);
  buggy.set(0,0);
  Serial.println("stoped");
  delay(2000);
}
