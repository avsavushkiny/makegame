#pragma once

#include <Arduino.h>
#include <U8g2lib.h> 
#include "mg.h"
#include "xbmp.h"

Joystick joy;
Graphics gfx;
Cursor crs1;
Button btn1;
Shortcut iconSapper, iconGears;
Shortcut shrt1;
Shortcut iconSpaceInvaders;

bool stateGame1 = false; //Pong
bool stateGame2 = false; //AI
bool gameState2 = false; //AI

#define WIDTH 128       // ширина дисплея
#define HEIGHT 64       // высота дисплея
#define UNIT_WIDTH 5    // ширина юнита
#define UNIT_HEIGHT 4   // высота юнита
#define STATUS_WIDTH 15 // ширина колонки статуса
#define X 0
#define Y 1

#define CTRL_TO 50
int ENEMY_TO = 200;
#define ROCKET_FLY_TO 50
#define ROCKET_FIRE_TO 500
int PLASMA_FLY_TO = 50;
#define PLASMA_FIRE_TO 500

#define ENEMIES_COLS 8                        // врагов в ряду
#define ENEMIES_ROWS 3                        // всего рядов
#define ENEMIES_Q (ENEMIES_COLS*ENEMIES_ROWS) // общее количество врагов

#define ENEMY_Y_LIMIT WIDTH - (UNIT_WIDTH + 5) * ENEMIES_COLS - STATUS_WIDTH - 5

static uint8_t spaceInvaders_bits[] = { //w32 h32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 
  0x00, 0x1F, 0xF8, 0x00, 0x00, 0x1C, 0x38, 0x00, 0x00, 0xFF, 0xFF, 0x00, 
  0x00, 0xFF, 0xFF, 0x00, 0x00, 0xE3, 0xC7, 0x00, 0xC0, 0xE3, 0xC7, 0x03, 
  0xC0, 0xE3, 0xC7, 0x03, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 
  0xF0, 0xFF, 0xFF, 0x0F, 0x70, 0xFF, 0xFF, 0x0E, 0x70, 0xFF, 0xFF, 0x0E, 
  0x70, 0x07, 0xE0, 0x0E, 0x70, 0x7F, 0xFE, 0x0E, 0x00, 0x7C, 0x3E, 0x00, 
  0x00, 0x7C, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

struct unit
{
    byte state = 1;
    byte coords[2];
};

unit player;
unit enemies[ENEMIES_Q];
unit rocket;
unit plasma; //поставить статус на ноль
//курсор не пропадает а уходит за экран
unsigned long t, ctrl_next, enemy_next, rocket_fire_next, rocket_fly_next, plasma_fire_next, plasma_fly_next;

byte enemy_drift_x = 0;
int enemy_drift_dir = 1;
byte score = 0;
byte life = 3;

// спрайт игрока
//static uint8_t player_sprite[] = {0x04, 0x0E, 0x1F, 0x0A,};
static uint8_t player_sprite2[] = {0x08, 0x08, 0x1C, 0x55, 0x7F, 0x7F, 0x2A,}; //w7 h7
// спрайт врагов
static uint8_t enemy_sprite[] = {0x15, 0x0E, 0x0E, 0x15,};

void enemyMove()
{
    if (enemy_drift_dir > 0 && enemy_drift_x < ENEMY_Y_LIMIT)
    {
        enemy_drift_x += 1;
    }
    else if (enemy_drift_dir < 0 && enemy_drift_x > 5)
        enemy_drift_x -= 1;
    else
        enemy_drift_dir = -1 * enemy_drift_dir;

    for (byte e = 0; e < ENEMIES_Q; e++)
        enemies[e].coords[X] += enemy_drift_dir;
}

void resetGame()
{
    gameState2 = false;

    for(byte e = 0; e < ENEMIES_Q; e++)
    {
        enemies[e].state = 1;
        enemies[e].coords[X] = 0;
        enemies[e].coords[Y] = 1;
        enemy_drift_x = 0;
        enemy_drift_dir = 1;
    }

    score = 0; life = 3;

    //joy.posX0 = 64;
    //joy.posY0 = 32;
}

void drawEnemyPlasma()
{
    // если статус плазмы - ложь, не отрисовываем её
    if (!plasma.state)
        return;
    u8g2.drawVLine(plasma.coords[X], plasma.coords[Y], 3);
}

void fireEnemyPlasma(byte idx)
{
    plasma.state = 1;
    plasma.coords[X] = enemies[idx].coords[X] + UNIT_WIDTH / 2;
    plasma.coords[Y] = enemies[idx].coords[Y] + UNIT_WIDTH;
}

void plasmaMove()
{
    // если статус плазмы - ложь, не обрабатываем её полёт
    if (!plasma.state)
        return;
    // проверяем, пересекает ли плазма контур игрока
    if (plasma.coords[X] > player.coords[X] &&
        plasma.coords[X] < player.coords[X] + UNIT_WIDTH &&
        plasma.coords[Y] > player.coords[Y])
    {
        // если пересекает, выключаем плазму и запускаем процедуру смерти игрока
        playerKill();
        return;
    }

    if (plasma.coords[Y] == HEIGHT)
    {
        plasma.state = 0;
    }
    plasma.coords[Y] += 1;
}

void enemyKill(byte idx)
{
    rocket.state = 0;
    enemies[idx].state = 0;
    score++;
    ENEMY_TO -= 11-ENEMIES_COLS;
    PLASMA_FLY_TO -= 1;
    if (score == ENEMIES_Q)
    {
        resetGame(); gameState2 = false;
    }
}

void drawInterface()
{
    //выводим счет и кол-во жизней
    if (score < 10)
    {
        gfx.print((String)score, 122, 8);
    }
    else gfx.print((String)score, 116, 8);

    gfx.print((String)life, 122, 20);
}

void drawPlayer()
{
    u8g2.drawXBMP(player.coords[X], player.coords[Y], 7, 7, player_sprite2); //5 4
}

void drawEnemies()
{
    for (byte e = 0; e < ENEMIES_Q; e++)
    {
        if (enemies[e].state)
        {
            u8g2.drawXBMP(enemies[e].coords[X], enemies[e].coords[Y], 5, 4, enemy_sprite);
        }
    }
}

void drawPlayerRocket()
{
    // если статус ракет - ложь, не отрисовываем её
    if ((!rocket.state))
    {
        rocket.coords[X] = player.coords[X] + 3;
        rocket.coords[Y] = player.coords[Y];
        return;
    }
    else u8g2.drawVLine(rocket.coords[X], rocket.coords[Y], 3);
}

void firePlayerRocket()
{
    rocket.state = 1;
    rocket.coords[X] = player.coords[X] + 3;
    rocket.coords[Y] = player.coords[Y] - 1;
}

void rocketMove()
{
    //if (joy.pressKeyB())
    //{
        // если статус ракет - ложь, не обрабатываем её полёт
        if (!rocket.state)
            return;
        // перебираем всех врагов
        for (byte e = 0; e < ENEMIES_Q; e++)
        {
            // проверяем, пересекает ли ракета в контур пришельца
            if (enemies[e].state &&
                rocket.coords[X] > enemies[e].coords[X] &&
                rocket.coords[X] < enemies[e].coords[X] + UNIT_WIDTH &&
                rocket.coords[Y] < enemies[e].coords[Y] + UNIT_HEIGHT)
            {
                // если попадает, выключаем ракету и пришельца из игры
                enemyKill(e);
                return;
            }
        }

        if (rocket.coords[Y] == 0)
        {
            rocket.state = 0;
        }
        rocket.coords[Y] -= 1;
    //}
}

void playerKill()
{
    plasma.state = 0;
    if (life > 0)
    {
        life--; // убавляем количество жизней
    }
    else if (life == 0)
    {
        resetGame();
    }
}

void beginGame()
{
    if (gameState2 == false)
    {
        player.coords[X] = 128 / 2;
        player.coords[Y] = 52;

        // начальные координаты врагов
        for (byte x = 0; x < ENEMIES_COLS; x++)
        { // в ряду 8 врагов
            for (byte y = 0; y < ENEMIES_ROWS; y++)
            { // всего 3 ряда врагов
                byte e = x + y * ENEMIES_COLS;
                enemies[e].coords[X] = 5 + x * (5 + UNIT_WIDTH); // 5
                enemies[e].coords[Y] = 5 + y * (3 + UNIT_WIDTH); // 3
            }
        }
        ctrl_next = millis() + CTRL_TO;
    }
}

void gameSpaceInvaders()
{
    beginGame();
    
    gameState2 = true;
    
    unsigned long t = millis();
    if (t > ctrl_next)
    {
        ctrl_next = t + CTRL_TO;

            joy.updatePositionX0Y0(0, 124, 0, 64);

        player.coords[X] = joy.posX0;
        player.coords[Y] = 52;

        u8g2.clearBuffer();     // очистка фрейма
        drawInterface();        // отрисовка очков и жизней
        drawPlayer();           // отрисовка игрока на фрейме
        drawEnemies();          // отрисовка врагов на фрейме
        drawEnemyPlasma();      // отрисовка плазмы врага
        drawPlayerRocket();     // отрисовка ракеты игрока
        u8g2.sendBuffer();      // вывод фрейма на дисплей
    }
    if (t > enemy_next)
    {
        enemy_next = t + ENEMY_TO;
        enemyMove();
    }
    if (t > plasma_fire_next && !plasma.state)
    {
        plasma_fire_next = t + PLASMA_FIRE_TO;
        byte e = random(0, ENEMIES_Q);
        if (!enemies[e].state)
        {
            while (!enemies[e].state)
            {
                e = random(0, ENEMIES_Q);
            }
        }
        byte idx = e;
        /*for (byte y = 0; y < ENEMIES_ROWS; y++)
        {
            for (byte x = 0; x < ENEMIES_COLS; x++)
            {
                e = x + (2 - y) * ENEMIES_COLS;
                // выбираем первого попавшегося живого врага в ближнем к игроку ряду
                if (enemies[e].state)
                {
                    idx = e;
                    break;
                }
            }
            if (idx < 255)
                break;
        }*/
        if (idx < 255)
            fireEnemyPlasma(idx);
    }
    if (t > plasma_fly_next)
    {
        plasma_fly_next = t + PLASMA_FLY_TO;
        plasmaMove();
    }
    if (t > rocket_fire_next && !rocket.state)
    {
        rocket_fire_next = t + ROCKET_FIRE_TO;
        firePlayerRocket();
    }
    if (t > rocket_fly_next)
    {
        rocket_fly_next = t + ROCKET_FLY_TO;
        rocketMove();
    }
}


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

int8_t xBall{63}, yBall{31}, xx = 2, yy = 1, score1{}, score2{}, posX00, posX11; 


void drawField()
{
    u8g2.drawHLine(0, 0, 128);
    u8g2.drawHLine(0, 63, 128);
    for (int i = 0; i <= 64; i+=6)
    {
        u8g2.drawVLine(64, i, 4);
    }
}
void DrawBall() {
  u8g2.drawBox(xBall, yBall, 4, 4); xBall += xx; yBall += yy;
  if(xBall >= 125) xx = -2;
  if(xBall <= 1) xx = 2;
  if(yBall >= 61) yy = -1;
  if(yBall <= 1) yy = 1;
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

  if ((xBall >= joy.posX0) && (xBall <= joy.posX0 + 5))
  {
    if((yBall >= joy.posY0) && (yBall <= joy.posY0 + 10))
    {
      xx = 2;
    }
  }
    if ((xBall + 3 >= joy.posX1) && (xBall <= joy.posX1))
  {
    if((yBall >= joy.posY1) && (yBall <= joy.posY1 + 10))
    {
      xx = -2;
    }
  }
    if ((score1 == 5) || (score2 == 5))
    {
      stateGame1 = false;

      score1 = 0; score2 = 0;
    }


}
void drawScore()
{
    String score1st{score1}, score2st{score2};
    gfx.print(score1st, 20, 10);
    gfx.print(score2st, 101, 10);
}
void DrawRackets() {
    posX00 = joy.posX0;   posX11 = joy.posX1;

  if (joy.posX0 >= 53) { posX00 = 0; }
  if (joy.posX1 >= 73) { posX11 = 122; }

  joy.updatePositionX0Y0(0, 53, 1, 53);
  u8g2.drawFrame(joy.posX0, joy.posY0, 5, 10);

  joy.updatePositionX1Y1(73, 122, 1, 53);
  u8g2.drawFrame(joy.posX1, joy.posY1, 5, 10);
}


void gamePong()
{
    stateGame1 = true;
    drawField(); DrawRackets(); Calculation(); DrawBall(); drawScore();
}

void desctop()
{
    joy.updatePositionX0Y0(0, 127, 0, 64);

    gfx.print("Move the cursor\nto the Pong game\nshortcut", 5, 10, 8, 5);
    iconSapper.shortcut(sapper_bits, 5, 30, gamePong, joy.posX0, joy.posY0);
    iconSpaceInvaders.shortcut(spaceInvaders_bits, 42, 30, gameSpaceInvaders, joy.posX0, joy.posY0);
    crs1.cursor(true, joy.posX0, joy.posY0);
}

void interfaceBoard()
{
    if ((stateGame1 == false) && (gameState2 == false)) gfx.render(desctop);
    if (stateGame1 == true) gfx.render(gamePong); //Pong
    if (gameState2 == true) gfx.render(gameSpaceInvaders); //AI
}