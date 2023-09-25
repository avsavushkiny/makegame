#include <mg.h>

Graphics gfx;
Timer tm1, tm2;

long int ttimer{};

void information()
{
    gfx.print("Turn the device over,\nthe LED should flash\nevery second.", 5, 10, 8, 5);
    gfx.print((String)ttimer, 5, 40, 8, 6);
}

void blinkOn()
{
    digitalWrite(25, true);
}

void blinkOff()
{
    digitalWrite(25, false);
}

void setup()
{
    gfx.initializationSystem();
    pinMode(25, OUTPUT);
}

void setup1()
{
    //Your code to run on the second core
}

void loop()
{
    ttimer = millis();

    gfx.render(information);

    tm1.timer(blinkOn, 500);
    tm2.timer(blinkOff, 1000);
}

void loop1()
{ 
    //Your code to run on the second core
}