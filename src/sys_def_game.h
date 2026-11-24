/*
  sys_def_game.h — Встроенная игра Pong (Atari 1972) под 128x60 со звуковыми эффектами Core 1
*/

#pragma once

#include "sys.h"
#include "sys_gfx.h"
#include "sys_sound.h"

namespace defGame
{
    void addEffectBall(int paddleSpeed);

    const int8_t SCREEN_OX = 0;
    const int8_t SCREEN_OY = 0;
    const int8_t SCREEN_WIDTH = MG_SCREEN_W - 1;
    const int8_t SCREEN_HEIGHT = MG_SCREEN_H - 1;
    const int8_t PADDLE_WIDTH = 3;
    const int8_t PADDLE_HEIGHT = 12;
    const int8_t PADDLE_PADDING = 6;
    const int8_t BALL_SIZE = 3;

    const float EFFECT_SPEED = 0.4f;
    const float MIN_Y_SPEED = 0.5f;
    const float MAX_Y_SPEED = 2.0f;

    inline float ballX = SCREEN_WIDTH / 2;
    inline float ballY = SCREEN_HEIGHT / 2;
    inline float ballSpeedX = 2.0f;
    inline float ballSpeedY = 1.0f;

    inline int paddleLocationA = 24;
    inline int paddleLocationB = 24;
    inline int lastPaddleLocationA = 24;
    inline int lastPaddleLocationB = 24;

    inline int scoreA = 0;
    inline int scoreB = 0;

    inline Timer ballTimer(25);

    inline void resetGame()
    {
        scoreA = 0; scoreB = 0;
        ballX = SCREEN_WIDTH / 2; ballY = SCREEN_HEIGHT / 2;
        ballSpeedX = 2.0f; ballSpeedY = 1.0f;
        paddleLocationA = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
        paddleLocationB = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
    }

    inline void drawField()
    {
        for (int i = 2; i < SCREEN_HEIGHT; i += 4) u8g2.drawVLine(SCREEN_WIDTH / 2, i, 2);
        u8g2.drawHLine(SCREEN_OX, SCREEN_OY, SCREEN_WIDTH + 1);
        u8g2.drawHLine(SCREEN_OX, SCREEN_HEIGHT, SCREEN_WIDTH + 1);
    }

    inline void drawRacket()
    {
        u8g2.drawBox(PADDLE_PADDING, paddleLocationA, PADDLE_WIDTH, PADDLE_HEIGHT);
        u8g2.drawBox(SCREEN_WIDTH - PADDLE_WIDTH - PADDLE_PADDING, paddleLocationB, PADDLE_WIDTH, PADDLE_HEIGHT);
    }

    inline void drawBall()
    {
        u8g2.drawBox((int)ballX, (int)ballY, BALL_SIZE, BALL_SIZE);
    }

    inline void drawScore()
    {
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(45, 10); u8g2.print(scoreA);
        u8g2.setCursor(76, 10); u8g2.print(scoreB);
    }

    inline void calculateMovementRackets()
    {
        int maxPaddleY = SCREEN_HEIGHT - PADDLE_HEIGHT - 1;
        paddleLocationA = constrain(paddleLocationA + sys.stick0Y() * 2, 1, maxPaddleY);
        paddleLocationB = constrain(paddleLocationB + sys.stick1Y() * 2, 1, maxPaddleY);
    }

    inline void calculateMovementBall()
    {
        if (!ballTimer.isReady()) return;

        ballX += ballSpeedX;
        ballY += ballSpeedY;

        int paddleSpeedA = paddleLocationA - lastPaddleLocationA;
        int paddleSpeedB = paddleLocationB - lastPaddleLocationB;

        if (ballY >= SCREEN_HEIGHT - BALL_SIZE)
        {
            ballY = SCREEN_HEIGHT - BALL_SIZE;
            ballSpeedY *= -1;
            sound.play(SND_MENU_MOVE);
        }
        else if (ballY <= 1)
        {
            ballY = 1;
            ballSpeedY *= -1;
            sound.play(SND_MENU_MOVE);
        }

        if (ballX <= PADDLE_PADDING + PADDLE_WIDTH && ballX >= PADDLE_PADDING && ballSpeedX < 0)
        {
            if (ballY + BALL_SIZE >= paddleLocationA && ballY <= paddleLocationA + PADDLE_HEIGHT)
            {
                ballX = PADDLE_PADDING + PADDLE_WIDTH;
                ballSpeedX *= -1;
                sound.play(SND_HIT);
                addEffectBall(paddleSpeedA);
            }
        }

        int rightPaddleX = SCREEN_WIDTH - PADDLE_WIDTH - PADDLE_PADDING;
        if (ballX + BALL_SIZE >= rightPaddleX && ballX <= rightPaddleX + PADDLE_WIDTH && ballSpeedX > 0)
        {
            if (ballY + BALL_SIZE >= paddleLocationB && ballY <= paddleLocationB + PADDLE_HEIGHT)
            {
                ballX = rightPaddleX - BALL_SIZE;
                ballSpeedX *= -1;
                sound.play(SND_HIT);
                addEffectBall(paddleSpeedB);
            }
        }

        if (ballX >= SCREEN_WIDTH - BALL_SIZE)
        {
            scoreA++;
            sound.play(SND_COIN);
            ballX = SCREEN_WIDTH / 2; ballY = SCREEN_HEIGHT / 2; ballSpeedX = -2.0f;
        }
        else if (ballX <= 0)
        {
            scoreB++;
            sound.play(SND_COIN);
            ballX = SCREEN_WIDTH / 2; ballY = SCREEN_HEIGHT / 2; ballSpeedX = 2.0f;
        }

        lastPaddleLocationA = paddleLocationA;
        lastPaddleLocationB = paddleLocationB;
    }

    inline void addEffectBall(int paddleSpeed)
    {
        float oldBallSpeedY = ballSpeedY;
        for (int effect = 0; effect < abs(paddleSpeed); effect++)
            ballSpeedY += (paddleSpeed > 0) ? EFFECT_SPEED : -EFFECT_SPEED;

        if (abs(ballSpeedY) < MIN_Y_SPEED)
        {
            ballSpeedY = (ballSpeedY >= 0) ? MIN_Y_SPEED : -MIN_Y_SPEED;
            if (ballSpeedY == 0) ballSpeedY = oldBallSpeedY;
        }
        ballSpeedY = constrain(ballSpeedY, -MAX_Y_SPEED, MAX_Y_SPEED);
    }
}
