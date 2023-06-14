#include <string.h>
#include <ESP32Servo.h>

#define IBUS_BUFFSIZE 32    
#define IBUS_MAXCHANNELS 10

static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static uint16_t rcValue[IBUS_MAXCHANNELS];

static boolean rxFrameDone;

struct RC_state{
  int x;
  int y;
  int failsafe; //there is a failsafe channel on the Receiever that estimates signal strength / tells us if the RC is connected.
};

class RC {
public:
  RC():
    state{0,0,0}
  {} //perhaps allow the user of you're code to specify the desired RC channels with an overloaded constructor (not necessary, but would be a nice toutch for completeness)
  
void print_state (RC_state state){

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

  void get_state(int x_channel, int y_channel, int failsafe_channel, RC_state& state){
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



RC_state state;

private: //i'd prefer not using global variables if possible - could get messy.
  
};



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




 