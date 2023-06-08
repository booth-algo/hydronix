#include "sensor.h"
bool called = 0;

void setup() { 
  Serial.begin(9600);
  Serial2.begin(600);
} 

void loop(){
  sensor a(A0,A0); 
  if(!called){
    Serial.println(a.magnetic());
    called = 1;
  }
  
}
