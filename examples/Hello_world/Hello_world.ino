#include <mg.h>

Graphics gfx;

void helloWorld()
{
    gfx.print("Hello world", 10, 10, 10, 3); // character spacing 3
    gfx.print("Hello world", 10, 20, 10, 4); // character spacing 4
    gfx.print("Hello world", 10, 30, 10, 5); // character spacing 5
    gfx.print("Hello world", 10, 40, 10, 6); // character spacing 6
    gfx.print("Hello world", 10, 50, 10, 7); // character spacing 7
    gfx.print("Hello world", 10, 60, 10, 8); // character spacing 8
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
    gfx.render(helloWorld);
}

void loop1()
{
  //Your code to run on the second core
}