#include "mg.h"

Joystick joy;
Graphics gfx;

int score1{};
int score2{};

int xBall{63};
int yBall{31};

int xx = 2;
int yy = 1;

int posY00;  
int posY11;

void DrawField() {
  u8g2.drawHLine(0, 0, 128);
  u8g2.drawHLine(0, 63, 128);
  for (int a = 0; a <= 64; a = a + 6) {
    u8g2.drawVLine(64, a, 4);
  }
}
void DrawRackets() {
  joy.updatePositionX0Y0(0, 53, 0, 53);
  u8g2.drawFrame(joy.posX0, joy.posY0, 5, 10);

  //posY00 = joy.posY0;   posY11 = joy.posY1;

  //if (joy.posY0 >= 54) { posY00 = 54; }
  //if (joy.posY1 >= 54) { posY11 = 54; }

  joy.updatePositionX1Y1(73, 127, 0, 53);
  u8g2.drawFrame(joy.posX1, joy.posY1, 5, 10);
}

void DrawBall() {
  u8g2.drawBox(xBall, yBall, 4, 4); xBall += xx; yBall += yy;
  if(xBall >= 125) xx = -2;
  if(xBall <= 1) xx = 2;
  if(yBall >= 61) yy = -1;
  if(yBall <= 1) yy = 1;
}
void Win()
{
if(score1 == 5)
{
  u8g2.clearBuffer();
  gfx.print("player 1 Win!", 30, 31);

}
if(score2 == 5)
{
  u8g2.clearBuffer();
  gfx.print("player 2 Win!", 30, 31); 
}
 
}
void DrawScore()
{
  String Score1st{score1}, Score2st{score2};
gfx.print(Score1st, 20, 10);
gfx.print(Score2st, 107, 10);
}
void Calculation()
{
    if (xBall <= 1)
    {
        score2++;
    }
    if (xBall >= 125)
    {
        score1++;
    }

if ((xBall >= 6) && (xBall <= 14))
    {
        if ((yBall >= joy.posY0) && (yBall <= joy.posY0 + 10))
        {
            xx = 2;
        }
    }
    if ((xBall >= 109) && (xBall <= 117))
    {
        if ((yBall >= joy.posY1) && (yBall <= joy.posY1 + 10))
        {
            xx = -2;
        }
    }
}
void gamePong() 
{
  DrawScore();
  DrawField();
  DrawRackets();
  DrawBall();
  Calculation();
  Win();
}

void setup() {
  gfx.initializationSystem();
}

void loop() {
  gfx.render(gamePong, 10);
}