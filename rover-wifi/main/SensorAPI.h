#include <HardwareSerial.h>

class sensor{
public:
  sensor(int age_, int magnetic_ ){
    pinMode(age_,INPUT);
    age_pin = age_;
    magnetic_pin = magnetic_;
  }

  String radio() {
    String name = "";
    int count = 0;
    bool start = 0;
    int start_time = micros();
    int timeout = 1000;

    while (count < 4 && (micros()-start_time < timeout)) {
      if(Serial2.available()){
        char inByte = Serial2.read();
        
        if (start) {
          name += inByte;
          count++;
        }
        
        else if(inByte == '#'){
          name += inByte;
          count++;
          start = 1;
        }

      }
    }

    return name;
  }

  String age() {
    int last_value = 0;
    double last_time = micros();
    double sum = 0;
    int count = 0;
    int start_time = micros();
    int timeout = 80000;
    while(count<10 && (micros()-start_time < timeout)){
      int current_value = digitalRead(age_pin);
      if(last_value == 0 && current_value == 1){
        double current_time = micros();
        double period = current_time - last_time;
        last_time = current_time;
        sum = sum + period;
        count++;
      }

      last_value = current_value;
    }

    return String(sum/100,0);
  }

  String magnetic(){ 
    int sensorValue = analogRead(magnetic_pin); 
    String polarity = "None"; // 0 refer to no magnetic field and voltage is VCC/2  

    if (sensorValue <= 3700) { 
      polarity = "N" ;
    } 

    else if (sensorValue >= 3850){ 
      polarity = "S" ;
    }

    return polarity;
  }

private:
  int age_pin;
  int magnetic_pin;

};
