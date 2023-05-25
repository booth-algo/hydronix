#include<Arduino.h>

/* this is the API for controlling the direction of the robot. 
This second version uses the nwe more powerful power management IC and esp32
uses pwm channels 0 - 3
Developed by Alex Charlton*/

struct Motor{

  Motor(int pwm_fwd,int pwm_bwd, int channel_fwd,int channel_bwd) : pwm_fwd(pwm_fwd), pwm_bwd(pwm_bwd), channel_fwd(channel_fwd), channel_bwd(channel_bwd) {}

  void begin(){
    int freq = 30000;
    pinMode(pwm_fwd, OUTPUT);
    pinMode(pwm_bwd, OUTPUT);
    ledcSetup(channel_fwd, freq,8);
    ledcSetup(channel_bwd, freq,8);
    ledcAttachPin(pwm_fwd, channel_fwd);
    ledcAttachPin(pwm_bwd, channel_bwd);
    set_speed(0);  
  }

  ~Motor(){
    set_speed(0);
    ledcDetachPin(pwm_fwd);
    ledcDetachPin(pwm_bwd);
  }
  
  int get_speed(){
    if(direction){
      return speed;
    }else{
      return (0 - speed);
    }
  }

  void set_speed(int new_speed){
    if(new_speed >=0){
      speed = new_speed;
      direction = true;
      ledcWrite(channel_fwd,speed);
      ledcWrite(channel_bwd,0);
    }else{
      speed = 0 - new_speed;
      direction = false;
      ledcWrite(channel_fwd,0);
      ledcWrite(channel_bwd,speed);
    }
  }

  private:
 
  int channel_fwd;
  int channel_bwd;
  int pwm_fwd;
  int pwm_bwd;
  bool direction;
  int speed;
};

struct Driving_system{

  Driving_system(int l_pwm_fwd, int l_pwm_rev,int r_pwm_fwd,int r_pwm_rev) :
    motor_R(r_pwm_fwd,r_pwm_rev, 0,1),
    motor_L(l_pwm_fwd,l_pwm_rev, 2,3),
    speed(0),
    direction(0)
  {}

  void begin(){
    motor_L.begin();
    motor_R.begin();
  }

  void set(int speed,int direction){
    
    if(speed > 255 | speed < -255){
      return;
    }
    
    int outside_speed = abs(speed);
    int inside_speed = abs(speed)-abs(direction);

    if (-255 > inside_speed){
      outside_speed -= inside_speed + 255;
      if(outside_speed > 255){
        outside_speed = 255;
      }
      inside_speed = -255;
    }

    if(speed < 0){
      outside_speed = - outside_speed;     
      inside_speed = - inside_speed;
    }

    if(direction > 0){
      motor_L.set_speed(outside_speed);
      motor_R.set_speed(inside_speed);
    }else{
      motor_R.set_speed(outside_speed);
      motor_L.set_speed(inside_speed);
    }
    Serial.print("L and R motor speed: "); Serial.print(motor_L.get_speed());Serial.print(" ");Serial.println(motor_R.get_speed());
  }

  private:
  int speed;
  int direction;
  Motor motor_L;
  Motor motor_R;
  
};

//global variables:
Driving_system buggy(33,32,26,25);

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
  buggy.set(0,225);
  Serial.println("right");
  delay(2000);
  buggy.set(0,0);
  Serial.println("stoped");
  delay(2000);
}
