#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid = "POCO";
const char* password = "test1234";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long previousTransmissionTime = 0;
const unsigned long transmissionInterval = 1000;

boolean motoronoff = false;
int motorspeed = 0;
String JSONtxt;

#include "joystick_page.h"
#include "functions.h"

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
    // Serial.print("L and R motor speed: "); Serial.print(motor_L.get_speed());Serial.print(" ");Serial.println(motor_R.get_speed());
  }

  private:
  int speed;
  int direction;
  Motor motor_L;
  Motor motor_R;
  
};

Driving_system motors(33,32,26,25);

void setup()
{

  motors.begin();
  Serial.begin(115200); 

  //-----------------------------------------------
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){Serial.print("."); delay(500);}
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  //-----------------------------------------------
  server.on("/", webpage);
  // server.on("/stop", HTTP_GET, handleMotorBrake);
  //-----------------------------------------------
  server.begin(); 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop(); 
  server.handleClient();

  // Rate limiting
  unsigned long currentMillis = millis();
  if (currentMillis - previousTransmissionTime >= transmissionInterval) {
    previousTransmissionTime = currentMillis;

    if(motoronoff == false) {
      motors.set(-motorspeed*225/100,0); //speed, direction (-225 to 225)
    } else {
      motors.set(motorspeed*225/100,0);
    }
    //-----------------------------------------------
    String motorstatus = "OFF";
    if(motoronoff == true) motorstatus = "ON";
    JSONtxt = "{\"motoronoff\":\""+motorstatus+"\"}";
    webSocket.broadcastTXT(JSONtxt);
  }
}

// need a handleNotFound()