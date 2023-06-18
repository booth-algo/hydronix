#include <string.h>
#include <ESP32Servo.h>

#define IBUS_BUFFSIZE 32    
#define IBUS_MAXCHANNELS 10

//member variables are preferable to static globals
static uint8_t ibusIndex = 0; 
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static uint16_t rcValue[IBUS_MAXCHANNELS];

static boolean rxFrameDone;

struct RC_state{
  int x;
  int y;
  int failsafe;
};

class RC {
public:
  RC():
    state()
  {} 
  
void print_state (RC_state state){ //debuging code should not be in the class definiton. (not really an issue)

  if (Serial.available())
  {

    Serial.print(state.x);
        Serial.print("     ");
        Serial.print(state.y);
        Serial.print("     ");
        if(state.failsafe > 950){
          Serial.print("Connected -> ");
          Serial.print(state.failsafe);
          Serial.println("     ");
        }else{
          Serial.print("Not connected -> ");
          Serial.print(state.failsafe);
          Serial.println("     ");
        }
  }
}

  void get_state(int x_channel, int y_channel, int failsafe_channel){ //i can implement the function pointers for you if you wish. do you want this function to be loop()? it also looks like these parameters should be defined in member data
    
    rxFrameDone = false;

    if (Serial.available())
    {
      uint8_t val = Serial.read();
  
      if (ibusIndex == 0 && val != 0x20)
      {
        ibusIndex = 0;
        return;
      }
      if (ibusIndex == 1 && val != 0x40)
      {
        ibusIndex = 0;
        return;
      }
  
      if (ibusIndex == IBUS_BUFFSIZE)
      {
        ibusIndex = 0;
        int high = 3;
        int low = 2;
  
        for (int i = 0; i < IBUS_MAXCHANNELS; i++)
        {
          rcValue[i] = (ibus[high] << 8) + ibus[low];
          high += 2;
          low += 2;
        }
        
        for (int i = 0; i < IBUS_MAXCHANNELS; i++){
          if(x_channel == i+1){
            state.x = map(rcValue[i], 1000, 2000, 1000, 2000);
          }
          if(y_channel == i+1){
            state.y = map(rcValue[i], 1000, 2000, 1000, 2000);
          }
           if(failsafe_channel == i+1){
            state.failsafe = map(rcValue[i], 1000, 2000, 1000, 2000);
          }
  
        }
        print_state(state);

      //to use the state, you need to call a function from here

      rxFrameDone = true;
      return;
      }
      else
      {
        ibus[ibusIndex] = val;
        ibusIndex++;
      }
    }
  }

private:

  RC_state state; //moved this to private
  
};

/*
void setup()
{
  Serial.begin(115200);
}

void loop()
{
  RC myRC;
  RC_state aState;               //we select pins x(1) y(3) failsafe(7)
  myRC.get_state(1,3,7, aState); //state is passed by reference
  //delay(20);
  //myRC.print_state(aState);
}
*/



 
