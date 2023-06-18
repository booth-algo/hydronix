#ifndef HYDRONIX_WEB_API
#define HYDRONIX_WEB_API

#include <functional>

#include <ArduinoJson.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include "HTML.h"

class Web_IO {
public:
  Web_IO(int polling_interval, std::function<void(String)> handleClientData,std::function<String(void)> getData, const char *SSID, const char *WIFI_PASSWORD) : 
    SSID(SSID),
    WIFI_PASSWORD(WIFI_PASSWORD),
    server(80), 
    webSocket(81), 
    polling_interval(polling_interval), 
    last_poll(millis()), 
    handleClientData(handleClientData),
    waiting(true),
    getData(getData)
  {}

  Web_IO(std::function<void(String)> handleClientData, std::function<String(void)> getData, const char *SSID, const char *WIFI_PASSWORD):
    Web_IO(500,handleClientData,getData, SSID, WIFI_PASSWORD)
  {}

  void begin() {
    setup_wifi(SSID, WIFI_PASSWORD);
    setup_web_server();
    setup_web_sockets();
  }

  void loop() {
     webLoop();
     if(millis() > last_poll + polling_interval || !waiting){
      waiting = true;
      last_poll = millis();
      String data = getData();
      pollWebPage(data);
     }
    }

private:
  void webLoop() {
    webSocket.loop();
    server.handleClient();
  }

  void pollWebPage(String data) { 
    int no_of_connections = webSocket.connectedClients(false);
    for(int i = 0; i < no_of_connections; i++){
      webSocket.sendTXT(0, data); 
    }
  }

  std::function<void(void)> webpage = [this]() {
    for (int i = 0; i < webSocket.connectedClients(false); i++) {
      webSocket.disconnect(i);
    }
    server.send(200, "text/html", webpageCode);
  };

  std::function<void(uint8_t num, WStype_t type, uint8_t *payload, size_t welength)>
      webSocketEvent = [this](uint8_t num, WStype_t type, uint8_t *payload, size_t welength) {
        waiting = false;
        String payloadString = (const char *)payload;

        if(type = WStype_TEXT){
          handleClientData(payloadString);
        }
      };

  void setup_wifi(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }

  void setup_web_server() {
    server.on("/", webpage);
    server.begin();
  }

  void setup_web_sockets() {
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
  }

  const char *SSID;
  const char *WIFI_PASSWORD;

  WebServer server;
  WebSocketsServer webSocket;

  std::function<void(String)> handleClientData;
  std::function<String(void)> getData;

  int polling_interval;
  int last_poll;
  bool waiting;
};

#endif
