#pragma once

#include <Arduino.h>
#include <U8g2lib.h> 
#include "mg.h"

extern Joystick joy;
extern Graphics gfx;

#define icon_MyNullApp_width 32
#define icon_MyNullApp_height 32
static unsigned char icon_MyNullApp_bits[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x0F, 0x10, 0x00, 0x00, 0x08, 
  0xD0, 0xFF, 0xFF, 0x0B, 0xD0, 0xFF, 0x49, 0x0A, 0xD0, 0xFF, 0x49, 0x0A, 
  0xD0, 0xFF, 0xFF, 0x0B, 0x10, 0x00, 0x00, 0x08, 0xD0, 0xFF, 0xFF, 0x0B, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 0x50, 0x12, 0x09, 0x0A, 
  0x50, 0xAA, 0x0A, 0x0A, 0x50, 0xAA, 0x0A, 0x0A, 0x50, 0x12, 0x09, 0x0A, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x86, 0x61, 0x0A, 0x50, 0x49, 0x92, 0x0A, 
  0x50, 0x49, 0x92, 0x0A, 0xD0, 0x30, 0x0C, 0x0B, 0x10, 0x00, 0x00, 0x08, 
  0xF0, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

struct schoolBoy
{
    short numberSchoolboy;
    short numberMove;
    bool state;
};

String arrSchoolboy[] =
    {
        "Pepperov Vova",
        "Aksenov Sergej",
        "Kirzhaev Dima",
        "Feoktistov ZHenya",
        "YUdanov Egor",
        "Rudnev Dima",
        "Lapshin Denis",
        "Kulagin Evgenij",
        "Butarev Lev",
        "Anisimov ZHenya",
        "SHelpakova Tanya",
        "Abajkin Gleb",
        "Loginov Andrej",
        "Haskhazyan Roma",
        "Bychkova Liza",
        "Korolev Ruslan",
        "Puzhaev Arsenij",
        "Pelih Bogdan",
        "SHevchenko Igor",
        "Vlasov Kolya"};

String arrMove[] =
    {
        "Podnimi ruki i zahodi",
        "Ulybnis i zahodi",
        "Stancuj i zahodi",
        "Daj pyat i zahodi",
        "Propusti hod",
        "Nachni snachala",
        "Ty segodnya Bratyunya"};

schoolBoy sb[sizeof(arrSchoolboy)/sizeof(arrSchoolboy[1])];
int countSchoolboy = sizeof(arrSchoolboy)/sizeof(arrSchoolboy[1]);
int countArrMove = sizeof(arrMove)/sizeof(arrMove[1]);

bool flagSetupGameSchoolboy = false;
void setupGames()
{
    if (flagSetupGameSchoolboy == false)
    {
        for (int a = 0; a <= countSchoolboy; a++)
        {
            sb[a].numberSchoolboy = a;                      // определяем ребенка
            sb[a].numberMove = random(0, countArrMove + 1); // определяем для детей действия
            sb[a].state = false;                            // все дети не прошли, пока что
        }
    }
}

void gamesS()
{
    for (schoolBoy all : sb)
    {
        Serial.println(arrSchoolboy[all.numberSchoolboy]);
        Serial.println(arrMove[all.numberMove]);
        Serial.println("");
    }
}


void games()
{

}

void gameSchoolboy()
{
    setupGames();

}