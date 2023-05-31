#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

//custom scripts
#include "DrivingAPI.h"
#include "functions.h"

//html page
#include "joystick_page.h"

const char* ssid = "POCO";
const char* password = "test1234";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long previousTransmissionTime = 0;
const unsigned long transmissionInterval = 1000;

boolean motoronoff = false;
int input_x = 0;
int input_y = 0;
String JSONtxt;

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
      motors.set(x,y); //speed, direction (-225 to 225)
    } else {
      motors.set(0,0);
    }
    //-----------------------------------------------
    String motorstatus = "OFF";
    if(motoronoff == true) motorstatus = "ON";
    JSONtxt = "{\"motoronoff\":\""+motorstatus+"\"}";
    webSocket.broadcastTXT(JSONtxt);
  }
}

// need a handleNotFound()
