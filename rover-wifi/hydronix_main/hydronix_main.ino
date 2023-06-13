#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

//custom scripts
#include "DrivingAPI.h"
#include "WebFunctions.h"

//html page
#include "joystick_page.h"

const char* SSID = "Hydronix";
const char* WIFI_PASSWORD = "1234567890";


struct WebInputData{
  int x;
  int y;
  bool radio;
};

//setup functions
void setup_wifi(char* ssid,char* password){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){Serial.print("."); delay(500);}
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}

void setup_web_server(){
  server.on("/", webpage);
  server.begin(); 
}

void setup_web_sockets(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

//global variables
Driving_system motors(33,32,26,25);
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup()
{
  Serial.begin(115200); 

  motors.begin();
  setup_wifi(SSID,WIFI_PASSWORD);
  setup_web_server();
  setup_web_sockets();
}

void loop()
{
  webSocket.loop(); 
  server.handleClient();

  Delay(1);
}

// need a handleNotFound()
