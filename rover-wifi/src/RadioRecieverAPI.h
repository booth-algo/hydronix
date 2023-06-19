#include <string.h>
#include <ESP32Servo.h>
#include <functional>

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
  RC(int x_channel, int y_channel, int failsafe_channel, std::function<void(RC_state)> handle_new_RX_reading) :
    handle_new_RX_reading(handle_new_RX_reading),
    x_channel(x_channel),
    y_channel(y_channel),
    failsafe_channel(failsafe_channel),
    ibusIndex(0),
    state()
  {} 

  RC(std::function<void(RC_state)> handle_new_RX_reading): RC(1,3,7,handle_new_RX_reading) {}
  
void print_state (RC_state state){ //debuging code should not be in the class definiton. (not really an issue)

  if (Serial.available())
  {
    Serial.print(state.x);
        Serial.print("     ");
        Serial.print(state.y);
        Serial.print("     ");
        if(state.failsafe > 950/5 - 300){
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

  void loop(){ //i can implement the function pointers for you if you wish. do you want this function to be loop()? it also looks like these parameters should be defined in member data

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
        
          state.x = map(rcValue[x_channel-1], 1000, 2000, 1000, 2000);
          state.y = map(rcValue[y_channel-1], 1000, 2000, 1000, 2000);
          state.failsafe = map(rcValue[failsafe_channel-1], 1000, 2000, 1000, 2000);

          double tmp; //map from 100 to -100          
          tmp = state.x/5 - 300;
          if(tmp>0){
            state.x = min(tmp, 100);
          }else{
            state.x = max(tmp, -100);
          }
          tmp = state.y/5 - 300;
          if(tmp>0){
            state.y = min(tmp, 100);
          }else{
            state.y = max(tmp, -100);
          }

        print_state(state);

        handle_new_RX_reading(state);

      }
      else
      {
        ibus[ibusIndex] = val;
        ibusIndex++;
      }
    }
  }

private:

  uint8_t ibusIndex; 
  uint8_t ibus[IBUS_BUFFSIZE];
  uint16_t rcValue[IBUS_MAXCHANNELS];

  std::function<void(RC_state)> handle_new_RX_reading;

  int x_channel;
  int y_channel;
  int failsafe_channel;
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



 
