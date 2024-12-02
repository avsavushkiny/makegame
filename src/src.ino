#include "mg.h"
#include "gamePong.h"
#include "gameAliens.h"
#include "gameSchoolboy.h"

Joystick joy;
Graphics gfx;
Cursor crs;
Button btn1;
Shortcut iconSapper, iconSpaceInvaders, iconSchoolboy;

void desktop()
{
    joy.updatePositionX0Y0(0, 127, 0, 64);

    gfx.print("Move the cursor\nto the Pong game\nshortcut", 5, 10, 8, 5);
    iconSapper.shortcut(sapper_bits, 5, 30, gamePong, joy.posX0, joy.posY0);
    iconSpaceInvaders.shortcut(spaceInvaders_bits, 42, 30, gameSpaceInvaders, joy.posX0, joy.posY0);
    iconSchoolboy.shortcut(icon_MyNullApp_bits, 79, 30, gameSchoolboy, joy.posX0, joy.posY0);
    
    crs.cursor(true, joy.posX0, joy.posY0);
}

void interfaceBoard()
{
    if ((stateGame1 == false) && (stateGame2 == false)) gfx.render(desktop);
    if (stateGame1 == true) gfx.render(gamePong); //Pong
    if (stateGame2 == true) gfx.render(gameSpaceInvaders); //AI
}

void setup()
{
    gfx.initializationSystem();
}


void loop()
{
    interfaceBoard();
}
