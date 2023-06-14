/* sory for not specifying how i wanted the code done sooner (i probably should have looked through you're code much sooner)

ideally i would like the code structured with each sensor seperated into its own class, with a class that handles all the individual sensor classes and can feed back the sensor data to the main.
here is a suggested structure (feel free to add private member functions, attributes and constructor parameters as you wish)

use a similar class for each sensor (not all member functions will be necesary for each sensor, the details can be abstracted out in the sensors class):

#include <vector>

struct sensor_reading_t{
  String sensor_name //this may not be necesary, so up to you if you wish to use it. if you do, please use sensor names as in the columbs in the webpage
  String reading; //i would appreciate sensor data being stored as strings - it saves having to do the conversion in the main (keeping it more simple)
  bool valid;
}

class sensor { //the clas for each sensor (pls dont actually call ur class sensor -> see sensors class attributes for example names)
  
public:
  
  sensor(int pin):
    pin_no(pin), //example of formating for constructor initialiser list
    reading("blah","",false) //idk if the default construct or sensor_readig_t works like this
  {} //avoid putting code in the constructor where possible (as you cant control which order they will be called in -> important as a lot of arduino interfaces may not have been initialised yet)

  sensor():
    sensor(7) //not necessary, but having a constructr that takes no arguments and uses default parameters is a nice toutch
  {}
  
  sensor_reading_t
  
  //feel free to add a destructor if you deem it necesary (although it probably isnt)
  
  void setup(){ //put all the code that you would put in you're constructor here. this will be called once
    //perhaps setup an interrupt if needed using: attachInterrupt(GPIOPin, ISR, Mode);
  }
  
  void loop(){ //put all the code that needs to be called in a loop. THIS MUST NOT HANG: DO NOT USE DELAY() OR INFINITE LOOPS
    //EG: perhaps check the contents of the Serial buffer
  }
  
  sensor_reading_t read_sensor(){//perhaps the only public member function needed for the magnetic field sensor
    return reading;
  }
  
private:
  
  void isr() {}; //handle interrupt if needed.
  
  sensor_reading_t to_sensor_reading_t(){} //also worth having private functions to keep the public ones as short and readable (this is an example, a function like this is probably best included in the sensor_readong_t struct)
  
  int pin_no; //use member data to track loops as you did in you're previous code
  sensor_reading_t reading; //idk if thats the best name for this variable, feel free to change it (would be nice to keep it consistent accross the 3 sensor classes though)
};

class sensors{ //combine all snesor classes together

public:
  
  sensors(): //add a destructor too if needed (its probably not, but freeing up pins is probably good practice)
    age(),
    name(),
    polarity()
  {}
  
  void begin(){
    age.begin();
    name.begin();
    polarity.begin();
  }
  
  void loop(){
    age.loop();
    name.loop();
    polarity.loop();
  }
  
  std::vector<sensor_reading_t> read_sensors(){
    //get all sensor readings. perhaps using a normal pointer array that is 3 long is a better idea here than std::vector as it is less overhed. feel free to use either, just remember to remove the include if you do
  }
  
private:
  
  //you probably wont need anny private member functoins, but feel free if you think you do.
  
  age age;
  name name;
  polarity polarity; //the 3 sensor classes, feel free to name them something else if you see fit
}

some more notes:
whoever is sorting out the radio code will need to talk to bruno to ensure that you dont have conflicts with rx pins (im pretty sure the esp32 has 3 of them hidden somewhere)
not all member functions will be needed for all sensors. feel free to implement as you wish (provided it wont break other code) so long as the interfaces are nice.
feel free to add test code (main() and loop()) so long as it gets commented out.

thanks for refactoring you're code, it will make the codebase so much more coherant (and hopefuly avoid lockups) Alex.
*/

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
    double current_time;
    double last_time = 0;
    double period;
    double sum = 0;
    int count = 0;
    double last_analog_value = 0;
    double current_analog_value;
    while(count<100){
      current_analog_value = analogRead(age_pin);
      if(last_analog_value < 100 && current_analog_value >= 100){
        current_time = millis();

        period = current_time - last_time;
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
