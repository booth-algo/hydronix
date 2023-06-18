/* this is the API for controlling the direction of the robot. 
V2.1 uses the new more powerful power management IC and esp32
- improved steering
uses pwm channels 0 - 3
Developed by Alex Charlton and Bruno Duaso*/

#ifndef DrivingSystem
#define DrivingSystem

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

  Driving_system(int l_pwm_fwd, int l_pwm_rev,int r_pwm_fwd,int r_pwm_rev) : // initialise as global variable if possible
    motor_R(r_pwm_fwd,r_pwm_rev, 0,1),
    motor_L(l_pwm_fwd,l_pwm_rev, 2,3),
    speed(0),
    direction(0)
  {}

  Driving_system():
    Driving_system(33, 32, 26, 25)
  {}

  void begin(){ // must be called in setup
    motor_L.begin();
    motor_R.begin();
  }

/*
  double mag_then_ceil (int x, int y){
    double speed;
    speed = sqrt(x^2 + y^2);
    speed = (speed * 255)/100;
    if(speed >= 255){
        return 255;
    }
    return speed;
  }
*/

  void set_pwm_diff(int speed,int direction){ // sets the speed and directin by ading a diferential to the set speed to each motor

    direction = direction / 2;
    int l_speed = speed + direction;
    int r_speed = speed - direction;

    if(abs(l_speed) + abs(r_speed) > 510){ //input out of range
      return;
    }

    if(l_speed > 255){
      r_speed -= l_speed - 255;
      l_speed = 255;
    }else if(r_speed > 255){
      l_speed -= r_speed - 255;
      r_speed = 255;
    }

    if(l_speed < -255){
      r_speed -= l_speed + 255;
      l_speed = -255;
    }else if(r_speed < - 255){
      l_speed -= r_speed + 255;
      r_speed = -255;
    }

    Serial.print("L speed:"); Serial.println(l_speed);
    Serial.print("R speed:"); Serial.println(r_speed);

    motor_L.set_speed(l_speed);
    motor_R.set_speed(r_speed);

    //Serial.print("L and R speed: "); Serial.print(l_speed);Serial.print(" ");Serial.println(r_speed);
  }

  void handle_joystick_zones(int y,int x){ // sets the speed and directin by ading a diferential to the set speed to each motor

    if((x<=25)&&(x>=-25)){
      set_pwm_diff(y,x);
    }else{
      set_pwm_diff(y,x/2);   
    }
  }  

  void set(int speed,int direction){ // user speed input function

    double tmp = (speed * 255) / 100;
    speed = tmp;
    tmp = ((direction * 2 * 255) / 100);
    direction = tmp;
    handle_joystick_zones(speed,direction);

  }

  private:
  int speed;
  int direction;
  Motor motor_L;
  Motor motor_R;
  
};

#endif