#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "POCO";
const char* password = "test1234";

// motor 1
#define motor1pin1 33
#define motor1pin2 14
#define channelA 0 // pwm channel
#define enable1 13 // enable movement pin

// motor 2
#define motor2pin1 27
#define motor2pin2 26
#define channelB 1 // pwm channel
#define enable2 25 // enable movement pin

const int freq = 30000;
const int resolution = 8;
int dutyCycle = 170;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

boolean motoronoff = false;
String JSONtxt;

#include "html_page.h"
#include "functions.h"

void setup()
{
  Serial.begin(115200); 
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(enable1, OUTPUT);
  pinMode(enable2, OUTPUT);

  ledcSetup(channelA, freq, resolution);
  ledcSetup(channelB, freq, resolution);
  ledcAttachPin(enable1, channelA);
  ledcAttachPin(enable2, channelB);
  ledcWrite(channelA, dutyCycle);
  ledcWrite(channelB, dutyCycle);

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
  //-----------------------------------------------
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  if(motoronoff == false) {
    digitalWrite(enable1, LOW);
    digitalWrite(enable2, LOW);
  } else {
    digitalWrite(enable1, HIGH);
    digitalWrite(enable2, HIGH);
  }
  //-----------------------------------------------
  String motorstatus = "OFF";
  if(motoronoff == true) motorstatus = "ON";
  JSONtxt = "{\"motoronoff\":\""+motorstatus+"\"}";
  webSocket.broadcastTXT(JSONtxt);
}

// need a handleNotFound()