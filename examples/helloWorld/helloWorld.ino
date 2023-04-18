#include <makegame.h>

Systems sys;
Graphics gfx;
Interface inf;
Terminal trm;

void hello()
{
    gfx.drawCursor(true,sys.joi0x(),sys.joi0y());
    gfx.print("Hello Bro!\nI am a MakeGame!\nWhat is your name?", 0, 20, 10, 6);
}


void setup()
{
    gfx.screen();
    inf.greetings();
    trm.terminal();
}

void loop()
{
    gfx.render(hello,0);

    if (sys.sw0())
    {
    }
}