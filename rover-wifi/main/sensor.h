#include <HardwareSerial.h>

class sensor{
public:
  sensor(int age, int magnetic ){
    age_pin = age;
    magnetic_pin = magnetic;
  }

  String radio() {
    String name = "";
    int count = 0;
    bool start = 0;

    while (count < 4) {
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

  double age() {
    double last_time = 0;
    double sum = 0;
    int count = 0;
    double last_analog_value = 0;
    while(count<100){
      double current_analog_value = analogRead(age_pin);
      if(last_analog_value < 100 && current_analog_value >= 100){
        double current_time = millis();

        double period = current_time - last_time;
        last_time = current_time;
        sum = sum + period;
        count++;
      }

      last_analog_value = current_analog_value;
    }

    return sum/100;
  }

  String magnetic(){ 
    double sensorValue = analogRead(magnetic_pin); 
    String polarity = "None"; // 0 refer to no magnetic field and voltage is VCC/2  

    if (sensorValue >= 538) { 
      polarity = "N" ;
    } 

    else if (sensorValue <= 530){ 
      polarity = "S" ;
    }

    return polarity;
  }

private:
  int age_pin;
  int magnetic_pin;

};
