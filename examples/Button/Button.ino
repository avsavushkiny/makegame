#include <mg.h>

Joystick joy;
Graphics gfx;
Interface inf;
Button btn1, btn2;
Cursor crs1;

void eventClickButton1()
{
    inf.message("Event Message", 1500);
}

void eventClickButton2()
{
    inf.message("Event Message", 1500);
}

void clickButtons()
{
    joy.updatePositionXY();
    crs1.cursor(true, joy.posX0, joy.posY0);

    gfx.print("Test Button", 10, 10, 8, 6);

    btn1.button("Button 1", 10, 25, eventClickButton1, joy.posX0, joy.posY0); //0
    btn2.button("Button 2", 60, 25, eventClickButton2, joy.posX0, joy.posY0); //0
}

void setup()
{
    gfx.initializationSystem();
}

void setup1()
{
    //Your code to run on the second core
}

void loop()
{
    gfx.render(clickButtons);
}

void loop1()
{  
    //Your code to run on the second core
}