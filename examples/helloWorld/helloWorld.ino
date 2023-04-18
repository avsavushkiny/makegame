#include "mg.h"

Systems sys;
Graphics gfx;
Interface inf;
Terminal trm;

void hello()
{
    gfx.drawCursor(true,sys.joi0x(),sys.joi0y());
    gfx.print("text\ntext2", 10, 10, 10, 6);
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
        Serial.println("hello");
        inf.messageInfo("game over\ntry again", 1500);
    }
}