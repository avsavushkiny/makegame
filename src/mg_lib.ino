#include "mg.h"

Joystick joy;
Graphics gfx;
Interface inf;
Button btn1, btn2, btn3;
Cursor crs1;
Shortcut iconSapper, iconGears;

const uint8_t sapper_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 
  0x00, 0x40, 0x03, 0x00, 0x00, 0x40, 0x03, 0x00, 0x00, 0xF0, 0x0F, 0x00, 
  0xC0, 0x0C, 0x30, 0x03, 0x60, 0x03, 0xC1, 0x06, 0xE0, 0x01, 0xC0, 0x07, 
  0xC0, 0x90, 0x99, 0x03, 0x40, 0x20, 0x24, 0x03, 0x40, 0x91, 0x96, 0x03, 
  0x20, 0x08, 0x6C, 0x07, 0x20, 0xCD, 0x99, 0x07, 0x38, 0x30, 0x66, 0x1F, 
  0x24, 0x25, 0xD6, 0x27, 0xFC, 0x2A, 0xBF, 0x3F, 0xB8, 0xC1, 0xEF, 0x1F, 
  0x60, 0x9F, 0xDF, 0x07, 0xE0, 0xAA, 0xEF, 0x07, 0xC0, 0x57, 0xF6, 0x03, 
  0xC0, 0xAD, 0xFD, 0x03, 0x80, 0xB7, 0xFF, 0x01, 0x80, 0xFF, 0xFF, 0x01, 
  0x40, 0xFF, 0xFF, 0x02, 0xC0, 0xFF, 0xFF, 0x03, 0x80, 0xF1, 0x8F, 0x01, 
  0x00, 0xC0, 0x02, 0x00, 0x00, 0xC0, 0x02, 0x00, 0x00, 0x80, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

int8_t ballX{1}, ballY{1}, ballXspeed{1}, ballYspeed{1}, score1{}, score2{};

void drawField()
{
    u8g2.drawHLine(0, 0, 128);
    u8g2.drawHLine(0, 63, 128);
    for (int i = 0; i <= 64; i+=6)
    {
        u8g2.drawVLine(64, i, 4);
    }
}

void calculate()
{
    //rebound
    if ((ballY >= 63 - 4) || (ballY <= 0))
    {
        ballYspeed *= -1;
    }

    //rebound
    if (ballX <= 0)
    {
        score2++; ballXspeed *= -1;
    }
    if (ballX >= 127 - 4)
    {
        score1++; ballXspeed *= -1;
    }

    //rebound player 1
    if ((ballX >= 10) && (ballX <= 10 + 4))
    {
        if ((ballY >= joy.posY0) && (ballY <= joy.posY0 + 10)) // y0
        {
            ballXspeed *= -1;
        }
    }
    //rebound player 2
    if ((ballX >= 113 - 4) && (ballX <= 113 + 4))
    {
        if ((ballY >= joy.posY1) && (ballY <= joy.posY1 + 10)) // y1
        {
            ballXspeed *= -1;
        }
    }

    //gfx.print((String)sys.s0x + " " + (String)sys.s0y, 30, 30);
}

void drawRackets()
{
    joy.updatePositionXY();
    u8g2.drawFrame(10, joy.posY0 , 4, 10); //y0
    u8g2.drawFrame(113, joy.posY1, 4, 10); //y1
    calculate();
}

void drawBall()
{
    ballX+=ballXspeed; ballY+=ballYspeed;
    u8g2.drawBox(ballX, ballY, 4, 4);
}

void drawScore()
{
    String score1st{score1}, score2st{score2};
    gfx.print(score1st, 20, 10);
    gfx.print(score2st, 101, 10);
}

void gamePong()
{
    drawField(); drawRackets(); drawBall(); drawScore();
}

void clickBtn1()
{
    inf.message("You clicked on\nthe Stick 0 button\n:D", 1500);
}

void clickBtn2()
{
    inf.message("Okay :((", 1500);
}

void helloBro()
{
    joy.updatePositionXY();
    btn1.button("HELLO", 5, 40, clickBtn1, joy.posX0, joy.posY0); //0
    iconSapper.shortcut(sapper_bits, 0, 0, NULL, joy.posX1, joy.posY1); //1
    gfx.print("hello\nworld\nuser!", 65, 10, 8, 4);
    crs1.cursor(true, joy.posX0, joy.posY0);
}

void setup()
{
    gfx.initializationSystem();
}

void setup1(){}

void loop()
{
    gfx.render(gamePong);
}

void loop1()
{   
}