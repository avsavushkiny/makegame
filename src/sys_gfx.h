/*
  sys_gfx.h — Графическая подсистема ST7565 (128x64, рабочая зона 128x60)
*/

#pragma once

#include <U8g2lib.h>
#include "sys.h"
#include "sys_xbmp.h"

#define WIDTH 128
#define HEIGHT 60
#define CHIP_GFX ST7565

inline U8G2_ST7565_ERC12864_F_4W_SW_SPI u8g2(U8G2_R0, 18, 19, 17, 16, 20);

class Gfx
{
private:
  unsigned long prevTime_0 = 0;

public:
  void screen()
  {
    u8g2.begin();
    u8g2.setContrast(0);
    sys.backlight(true);
  }

  void render(void (*ptr_draw_fn)(), int timeDelay = 0)
  {
    if (timeDelay <= 0)
    {
      u8g2.clearBuffer();
      ptr_draw_fn();
      u8g2.sendBuffer();
      return;
    }

    uint32_t endTime = millis() + timeDelay;
    do
    {
      u8g2.clearBuffer();
      ptr_draw_fn();
      u8g2.sendBuffer();
    } while (millis() < endTime);
  }

  void clear()
  {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
  }

  void print(String text, int x, int y, int lii = 10, int chi = 6)
  {
    u8g2.setFont(u8g2_font_6x10_tr);
    int sizeText = text.length();
    int yy = 0;
    int xx = 0;

    for (int i = 0; i < sizeText; i++)
    {
      if (text[i] == '\n')
      {
        yy += lii;
        xx = 0;
        continue;
      }
      u8g2.setCursor(xx + x, yy + y);
      u8g2.print(text[i]);
      xx += chi;
    }
  }

  bool winkPrint(void (*ptr_fn)(String, int, int), String text, int x, int y, int interval)
  {
    unsigned long currTime = millis();
    if (currTime - prevTime_0 >= (unsigned long)interval)
    {
      if (currTime - prevTime_0 >= (unsigned long)(interval * 2))
      {
        prevTime_0 = currTime;
      }
      return false;
    }
    else
    {
      ptr_fn(text, x, y);
      return true;
    }
  }

  bool drawCursor(bool stateCursor)
  {
    if (stateCursor)
    {
      u8g2.setDrawColor(2);
      u8g2.setBitmapMode(1);
      u8g2.drawXBMP(sys.joi0x(MG_SCREEN_W - cursor_w), sys.joi0y(MG_SCREEN_H - cursor_h), cursor_w, cursor_h, cursor);
      u8g2.setDrawColor(1);
      u8g2.setBitmapMode(0);
      return true;
    }
    return false;
  }
};

inline Gfx gfx;

inline void printf(String text, int x, int y)
{
  gfx.print(text, x, y, 10, 6);
}
