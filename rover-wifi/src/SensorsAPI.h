/*
This is V1.0 of our sensors code, which curently doesnt have the abstractions layed our as required for time related reasons
contributers: David Deng, Yixu Pam, Alex Charlton 
*/

#include <HardwareSerial.h>
#include <vector>

struct sensor_t{

  sensor_t(String reading, bool valid) :
    reading(reading),
    valid(valid)
  {}

  sensor_t() : sensor_t("",0){}

  String reading;
  bool valid;
};

class sensors{
public:

  sensors(int age_, int magnetic_ ):
    age_pin(age_),
    magnetic_pin(magnetic_)
  {}

  void begin(){
    pinMode(age_pin,INPUT);
  }

  std::vector<sensor_t> read_sensors(){
    std::vector<sensor_t> output;
    sensor_t tmp;

    output.push_back(age());
    output.push_back(radio());
    output.push_back(magnetic());

    return output;
  }

  sensor_t radio() {
    String name = "---";
    int count = 0;
    bool start = 0;
    int start_time = millis();
    int timeout = 100;

    while (count < 4) {

      if (millis()-start_time > timeout){
        return sensor_t("---",false);
      }

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

    return sensor_t(name,true);
  }

  sensor_t age() {
    int last_value = 0;
    double last_time = micros();
    double sum = 0;
    int count = 0;
    int start_time = micros();
    int timeout = 80000;
    while(count<10){

      if (micros()-start_time > timeout){
        return sensor_t("---",false);
      }

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

    return sensor_t(String(sum/90,0),true);
  }

  sensor_t magnetic(){ 
    int sensorValue = analogRead(magnetic_pin); 
    String polarity = "---"; // 0 refer to no magnetic field and voltage is VCC/2  

    if (sensorValue <= 3700) {
      return sensor_t("N",true);
    } 

    else if (sensorValue >= 3850){ 
      return sensor_t("S",true);
    }

  return sensor_t("---",false);
  }

private:
  int age_pin;
  int magnetic_pin;

};
