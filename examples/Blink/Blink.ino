#include <mg.h>

Graphics gfx;

void information()
{
    gfx.print("Turn the device over,\nthe LED should flash\nevery second.", 5, 10, 8, 5);
}

void setup()
{
    gfx.initializationSystem();
}

void setup1()
{
    pinMode(25, OUTPUT);
}

void loop()
{
    gfx.render(information);
}

void loop1()
{ 
    digitalWrite(25, true);
    delay(1000);
    digitalWrite(25, false);
    delay(1000);
}