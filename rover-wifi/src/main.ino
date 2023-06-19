#include <functional>
#include <ArduinoJson.h>

// custom scripts
#include "DrivingAPI.h"
#include "WebAPI.h"
#include "RadioRecieverAPI.h"
#include "SensorsAPI.h"

// html page

const char *SSID = "SHOUT DUCKS FOR PASSWORD";
const char *WIFI_PASSWORD = "1234567890";

struct clientData{

  clientData(int x,int y,bool radio): x(x), y(y), radio(radio){}

  int x;
  int y;
  bool radio;
};

class HydronixRover{
  public:

  HydronixRover(const char *SSID, const char *WIFI_PASSWORD):
    radioControl(handleRCData),
    joystick(false),
    motors(),
    buggyWebServer(handleClientData, getData, SSID, WIFI_PASSWORD), //can also use the other constructor (int, function, function, const char*, const char*) to set the polling rate
    clientState(0,0,false)
  {
    for(int i = 0; i < 3; i++){
      sensorData.push_back(sensor_t("---",false));
    }
  }

  void begin(){
    motors.begin();
    buggyWebServer.begin();
  }

  void loop(){
    buggyWebServer.loop();
  }

  private:

  std::function<void(RC_state)> handleRCData = [this](RC_state state){

    if(state.failsafe < 0){
      return;
    }

    if(!joystick){
      return;
    }

    motors.set(state.y,state.x);
  };

  std::function<void(String)> handleClientData = [this](String payloadString){

    DynamicJsonDocument doc(128);
    deserializeJson(doc, payloadString);

    int x = doc["x"];
    int y = doc["y"];
    joystick = doc["joystick"];

    if(!joystick){ //renaming joystick to RX would probs be smart
      motors.set(-y, x);
    }
  };
  
  std::function<String(void)> getData = [this](){

    StaticJsonDocument<200> doc;
    String output;
    
    std::vector<String> JSON_params;
    JSON_params.push_back("Age");
    JSON_params.push_back("Name");
    JSON_params.push_back("Gender"); //there must be a better way than this

    for(int i = 0; i < 3; i++){
      if(sensorData[i].valid){
        doc[JSON_params[i]] = sensorData[i].reading; 
      }
    }

    serializeJson(doc, output);
    return output;
  };

  Driving_system motors;
  Web_IO buggyWebServer;
  bool joystick;
  RC radioControl;

  clientData clientState;

  std::vector<sensor_t> sensorData;
};

HydronixRover rover(SSID,WIFI_PASSWORD);

void setup() {
  Serial.begin(115200);

  rover.begin();

  Serial.println("setup complete");
}

void loop() {
  rover.loop();
}
