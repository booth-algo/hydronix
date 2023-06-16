#include <functional>
#include <ArduinoJson.h>

// custom scripts
#include "DrivingAPI.h"
#include "WebAPI.h"

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
    motors(),
    buggyWebServer(handleClientData, getData, SSID, WIFI_PASSWORD), //can also use the other constructor (int, function, function, const char*, const char*) to set the polling rate
    clientState(0,0,false)
  {
    for(int i = 0; i < 3; i++){
      sensorDataNull.push_back(true);
      sensorData.push_back("placeholder");
    }
  }

  void begin(){
    motors.begin();
    buggyWebServer.begin();

    //TODO: remove following code, it's for testing only
    sensorDataNull[0] = false;
    sensorDataNull[1] = false;
    sensorDataNull[2] = false;
    String tmp = "43";
    sensorData[0] = tmp;
    tmp = "linus";
    sensorData[1] = tmp;
    tmp = "North";
    sensorData[2] = tmp;
  }

  void loop(){
    buggyWebServer.loop();
  }

  private:

  std::function<void(String)> handleClientData = [this](String payloadString){

    DynamicJsonDocument doc(128);
    deserializeJson(doc, payloadString);

    int x = doc["x"];
    int y = doc["y"];
    bool joystick = doc["joystick"];
    motors.set(-y, x);

  };
  
  std::function<String(void)> getData = [this](){

    StaticJsonDocument<200> doc;
    String output;
    
    std::vector<String> JSON_params;
    JSON_params.push_back("Age");
    JSON_params.push_back("Name");
    JSON_params.push_back("Gender"); //there must be a better way than this

    for(int i = 0; i < 3; i++){
      if(!sensorDataNull[i]){
        doc[JSON_params[i]] = sensorData[i]; 
      }
    }

    serializeJson(doc, output);
    return output;
  };

  Driving_system motors;
  webIO buggyWebServer;

  clientData clientState;

  std::vector<String> sensorData;
  std::vector<bool> sensorDataNull;
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
