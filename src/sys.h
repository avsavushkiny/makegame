/*
  sys.h — Аппаратная абстракция консоли RP2040:
  - Автокалибровка стиков (пины 26, 27, 28, 29)
  - Кнопки SW0 (пин 6), SW1 (пин 7), системный выход iSw (SW0 + SW1)
  - Подсветка дисплея (пин 8)
  - Неблокирующие таймеры Timer на millis()
*/

#pragma once

#include <Arduino.h>

#define MG_SCREEN_W 128
#define MG_SCREEN_H 60

class Systems
{
private:
  int centerY0 = 2048;
  int centerY1 = 2048;
  int centerX0 = 2048;
  int centerX1 = 2048;

  const int8_t PIN_JOI_0Y = 27;
  const int8_t PIN_JOI_1Y = 28;
  const int8_t PIN_JOI_X0 = 26;
  const int8_t PIN_JOI_X1 = 29;

  const int8_t PIN_SW0 = 6;
  const int8_t PIN_SW1 = 7;
  const int8_t PIN_BACKLIGHT = 8;

  int yJoi0 = MG_SCREEN_H / 2;
  int yJoi1 = MG_SCREEN_H / 2;
  int xJoi0 = MG_SCREEN_W / 2;
  int xJoi1 = MG_SCREEN_W / 2;

  unsigned long lastPressSw0 = 0;
  unsigned long lastPressSw1 = 0;
  unsigned long lastActivityTime = 0;
  const unsigned long DEBOUNCE_MS = 180;

public:
  void begin()
  {
    pinMode(PIN_SW0, INPUT_PULLUP);
    pinMode(PIN_SW1, INPUT_PULLUP);
    pinMode(PIN_BACKLIGHT, OUTPUT);

    analogReadResolution(12);
    delay(20);
    calibrateSticks();
    lastActivityTime = millis();
  }

  void calibrateSticks()
  {
    long sumX0 = 0, sumY0 = 0, sumX1 = 0, sumY1 = 0;
    for (int i = 0; i < 16; i++)
    {
      sumX0 += analogRead(PIN_JOI_X0);
      sumY0 += analogRead(PIN_JOI_0Y);
      sumX1 += analogRead(PIN_JOI_X1);
      sumY1 += analogRead(PIN_JOI_1Y);
      delay(2);
    }
    centerX0 = sumX0 / 16;
    centerY0 = sumY0 / 16;
    centerX1 = sumX1 / 16;
    centerY1 = sumY1 / 16;
  }

  int getStickAxis(int8_t pin, int center, bool invert = false)
  {
    int val = analogRead(pin);
    int diff = val - center;
    if (invert) diff = -diff;

    const int DEADZONE = 220;
    const int FAST_ZONE = 1100;

    int res = 0;
    if (diff < -FAST_ZONE) res = -2;
    else if (diff < -DEADZONE) res = -1;
    else if (diff > FAST_ZONE) res = 2;
    else if (diff > DEADZONE) res = 1;

    if (res != 0) lastActivityTime = millis();
    return res;
  }

  int stick0X() { return getStickAxis(PIN_JOI_X0, centerX0, true); }
  int stick0Y() { return getStickAxis(PIN_JOI_0Y, centerY0, false); }
  int stick1X() { return getStickAxis(PIN_JOI_X1, centerX1, true); }
  int stick1Y() { return getStickAxis(PIN_JOI_1Y, centerY1, false); }

  int joi0y(int maxLimit = MG_SCREEN_H - 1)
  {
    yJoi0 = constrain(yJoi0 + stick0Y(), 0, maxLimit);
    return yJoi0;
  }

  int joi1y(int maxLimit = MG_SCREEN_H - 1)
  {
    yJoi1 = constrain(yJoi1 + stick1Y(), 0, maxLimit);
    return yJoi1;
  }

  int joi0x(int maxLimit = MG_SCREEN_W - 1)
  {
    xJoi0 = constrain(xJoi0 + stick0X(), 0, maxLimit);
    return xJoi0;
  }

  int joi1x(int maxLimit = MG_SCREEN_W - 1)
  {
    xJoi1 = constrain(xJoi1 + stick1X(), 0, maxLimit);
    return xJoi1;
  }

  void resetCoords()
  {
    xJoi0 = xJoi1 = MG_SCREEN_W / 2;
    yJoi0 = yJoi1 = MG_SCREEN_H / 2;
  }

  bool sw0()
  {
    bool pressed = (digitalRead(PIN_SW0) == LOW);
    if (pressed) lastActivityTime = millis();
    return pressed;
  }

  bool sw1()
  {
    bool pressed = (digitalRead(PIN_SW1) == LOW);
    if (pressed) lastActivityTime = millis();
    return pressed;
  }

  bool sw0Pressed()
  {
    if (sw0() && (millis() - lastPressSw0 > DEBOUNCE_MS))
    {
      lastPressSw0 = millis();
      return true;
    }
    return false;
  }

  bool sw1Pressed()
  {
    if (sw1() && (millis() - lastPressSw1 > DEBOUNCE_MS))
    {
      lastPressSw1 = millis();
      return true;
    }
    return false;
  }

  // Одновременное зажатие двух кнопок — выход в главное меню
  bool iSw() { return sw0() && sw1(); }

  // Время бездействия пользователя (для скринсейвера)
  unsigned long getIdleTime() const { return millis() - lastActivityTime; }
  void resetIdleTimer() { lastActivityTime = millis(); }

  void backlight(bool state)
  {
    digitalWrite(PIN_BACKLIGHT, state ? HIGH : LOW);
  }
};

inline Systems sys;

/* Класс неблокирующего таймера */
class Timer
{
private:
  unsigned long prevTime = 0;
  unsigned long intervalMs = 100;

public:
  Timer(unsigned long interval = 100) : intervalMs(interval) {}
  void setInterval(unsigned long interval) { intervalMs = interval; }

  bool isReady()
  {
    unsigned long currTime = millis();
    if (currTime - prevTime >= intervalMs)
    {
      prevTime = currTime;
      return true;
    }
    return false;
  }
};
