#include <string.h>
#include <ESP32Servo.h>

#define IBUS_BUFFSIZE 32    
#define IBUS_MAXCHANNELS 10

static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static uint16_t rcValue[IBUS_MAXCHANNELS];

static boolean rxFrameDone;

int ch_width_1;
int ch_width_2;
int ch_width_3;
int ch_width_4;
int ch_width_5;
int ch_width_6;
int ch_width_7;
int ch_width_8;
int ch_width_9;
int ch_width_10;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  readRx();
}

void readRx()
{
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

      ch_width_1 = map(rcValue[0], 1000, 2000, 1000, 2000);
      //ch1.writeMicroseconds(ch_width_1);
      Serial.print(ch_width_1);
      Serial.print("     ");

      ch_width_2 = map(rcValue[1], 1000, 2000, 1000, 2000);
      //ch2.writeMicroseconds(ch_width_2);
      Serial.print(ch_width_2);
      Serial.print("     ");

      ch_width_3 = map(rcValue[2], 1000, 2000, 1000, 2000);
      //ch3.writeMicroseconds(ch_width_3);
      Serial.print(ch_width_3);
      Serial.print("     ");

      ch_width_4 = map(rcValue[3], 1000, 2000, 1000, 2000);
      //ch4.writeMicroseconds(ch_width_4);
      Serial.print(ch_width_4);
      Serial.print("     ");

      ch_width_5 = map(rcValue[4], 1000, 2000, 1000, 2000);
      //ch5.writeMicroseconds(ch_width_5);
      Serial.print(ch_width_5);
      Serial.print("      ");

      ch_width_6 = map(rcValue[5], 1000, 2000, 1000, 2000);
      //ch6.writeMicroseconds(ch_width_6);
      Serial.print(ch_width_6);
      Serial.print("      ");

      ch_width_7 = map(rcValue[6], 1000, 2000, 1000, 2000);
      //ch7.writeMicroseconds(ch_width_7);
      Serial.print(ch_width_7);
      Serial.print("      ");

      ch_width_8 = map(rcValue[7], 1000, 2000, 1000, 2000);
      //ch8.writeMicroseconds(ch_width_8);
      Serial.print(ch_width_8);
      Serial.print("     ");

      ch_width_9 = map(rcValue[8], 1000, 2000, 1000, 2000);
      //ch9.writeMicroseconds(ch_width_9);
      Serial.print(ch_width_9);
      Serial.print("     ");

      ch_width_10 = map(rcValue[9], 1000, 2000, 1000, 2000);
      //ch10.writeMicroseconds(ch_width_10);
      Serial.print(ch_width_10);
      Serial.println("     ");

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
