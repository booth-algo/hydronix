/* this is the API for controlling the direction of the robot. 
V2.1 uses the new more powerful power management IC and esp32
- improved steering
uses pwm channels 0 - 3
Developed by Alex Charlton with the help of Bruno Duaso*/

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

  void begin(){ // must be called in setup
    motor_L.begin();
    motor_R.begin();
  }

  double get_speed (int x, int y){
    double speed;
    speed = sqrt(x^2 + y^2);
    speed = (speed * 255)/100;
    if(speed >= 255){
        return 255;
    }
    return speed;
}

  void set_pwm_diff(int speed_alex,int direction){ // sets the speed and directin by ading a diferential to the set speed to each motor
    int x = direction;
    int y = speed_alex;
    double tmp = get_speed(x, y);
    int speed = tmp;
    int lMotor = 0;
    int rMotor = 0;

    if((x<=25)&&(x>=-25)){
        if((y<=25)&&(y>=-25)){
            //stand still
        }else{
            //rotate
            if(x>0){
                rMotor = -speed;
                lMotor = speed;
            }else{
                rMotor = speed;
                lMotor = -speed;
            }
        }
    }else{
        //move fowards/backwards
        if(x>0){
            if(y>0){
                //first quadrant
                lMotor = speed;
                rMotor = (1-x/200)*speed; //same as doing (100-(x/2))/100 times speed   -> x/2 is an arbitrary value, we can also try 2x/3 or others
            }else{
                //fourth quadrant
                lMotor = -speed;
                rMotor = -(1-x/200)*speed;
            }
        }else{
            if(y>0){
                //second quadrant
                lMotor = (1+x/200)*speed;
                rMotor = speed;
            }else{
                //third quadrant
                lMotor = -(1+x/200)*speed;
                rMotor = -speed;
            }
        
        }

    }

    motor_L.set_speed(lMotor);
    motor_R.set_speed(rMotor);

    Serial.print("L and R speed: "); Serial.print(lMotor);Serial.print(" ");Serial.println(rMotor);
  }

  void set(int speed,int direction){ // user speed input function

    double tmp = (speed * 255) / 100;
    speed = tmp;
    tmp = (direction * 255) / 100;
    direction = tmp;
    set_pwm_diff(speed,direction);

  }

  private:
  int speed;
  int direction;
  Motor motor_L;
  Motor motor_R;
  
};

#endif
