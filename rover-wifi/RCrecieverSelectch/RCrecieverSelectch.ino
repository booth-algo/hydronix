  #include <string.h>
  #include <Servo.h>

  #define IBUS_BUFFSIZE 32    
  #define IBUS_MAXCHANNELS 10

  static uint8_t ibusIndex = 0;
  static uint8_t ibus[IBUS_BUFFSIZE] = {0};
  static uint16_t rcValue[IBUS_MAXCHANNELS];

  static boolean rxFrameDone;

struct RC_state {
  int x;
  int y;
  int failsafe;
};

class RC {
public:
  RC() :
    state{ 0, 0, 0 }
  {}

  void setup() {}

  void loop() {
    readRx();
  }

  RC_state get_RC_state() {
    return state;
  }

private:
  RC_state state;

  void readRx() {
    rxFrameDone = false;

    if (Serial.available()) {
      uint8_t val = Serial.read();

      if (ibusIndex == 0 && val != 0x20) {
        ibusIndex = 0;
        return;
      }
      if (ibusIndex == 1 && val != 0x40) {
        ibusIndex = 0;
        return;
      }

      if (ibusIndex == IBUS_BUFFSIZE) {
        ibusIndex = 0;
        int high = 3;
        int low = 2;

        for (int i = 0; i < IBUS_MAXCHANNELS; i++) {
          rcValue[i] = (ibus[high] << 8) + ibus[low];
          high += 2;
          low += 2;
        }

          state.x = map(rcValue[0], 1000, 2000, 1000, 2000);
          state.y = map(rcValue[2], 1000, 2000, 1000, 2000);
          state.failsafe = map(rcValue[6], 1000, 2000, 1000, 2000);

          double tmp;
          //maps it from 100, to -100
          
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
        //tmp = state.failsafe/5 -
        //state.failsafe = tmp;

        
        Serial.print(state.x);
        Serial.print("     ");
        Serial.print(state.y);
        Serial.print("     ");



        if (state.failsafe > 950) {
          Serial.print("Connected -> ");
          Serial.print(state.failsafe);
          Serial.println("     ");
        }
        else {
          Serial.print("Not connected -> ");
          Serial.print(state.failsafe);
          Serial.println("     ");
        }

        rxFrameDone = true;
        return;
      }
      else {
        ibus[ibusIndex] = val;
        ibusIndex++;
      }
    }
  }
};

RC myRC;

void setup() {
  Serial.begin(115200);
  myRC.setup();
}

void loop() {
  myRC.loop();
  RC_state state = myRC.get_RC_state();
  // Perform actions based on the received state
}