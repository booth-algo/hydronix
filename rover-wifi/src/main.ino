#include <functional>
#include <ArduinoJson.h>

// custom scripts
#include "DrivingAPI.h"
#include "WebAPI.h"

// html page

const char *SSID = "SHOUT DICKS FOR PASSWORD";
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
    motors(33, 32, 26, 25),
    buggyWebServer(handleClientData, getData, SSID, WIFI_PASSWORD), //can also use the other constructor (int, function, function, const char*, const char*) to set the polling rate
    clientState(0,0,false)
  {}

  void begin(){
    motors.begin();
    buggyWebServer.begin();
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
    motors.set(-y,-x);

  };
  
  //TODO: this function should format sensor data
  std::function<String(void)> getData = [this](){
    return R"===({"age":69,"name":"hello","Gender":"N"})===";
  };

  Driving_system motors;
  webIO buggyWebServer;

  clientData clientState;
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
