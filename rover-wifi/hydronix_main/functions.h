
#ifndef WebFunctions
#define WebFunctions

//handle function: send webpage to client
void webpage()
{
  server.send(200,"text/html", webpageCode);
}
//function process event: new data received from client
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t welength)
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);

  if(type == WStype_TEXT) //receive text from client 
  {
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var= ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val= ");
    Serial.println(val);
    Serial.println(" ");

    DynamicJsonDocument doc(256);  // Adjust the size as per your requirements
    deserializeJson(doc, var);

    int x = doc["x"];
    int y = doc["y"];
    int speed = doc["speed"];
    int angle = doc["angle"];

    Serial.print("x = ");
    Serial.println(x);
    Serial.print("y = ");
    Serial.println(y);
    Serial.print("speed = ");
    Serial.println(speed);
    Serial.print("angle = ");
    Serial.println(angle);

      motoronoff = false;
      input_x = x;
      input_y = y;
      if(y<0) motoronoff = true;
      if(x==0 && y==0) motorspeed = 0; //error here

    
  }
}

#endif