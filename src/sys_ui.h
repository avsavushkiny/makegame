/*
  sys_ui.h — Библиотека классов пользовательского интерфейса (UI)
  - UIElement (базовый абстрактный класс)
  - Label (текстовая метка)
  - ProgressBar (шкала здоровья / датчиков)
  - ButtonUI (экранная кнопка с фокусом)
  - MenuList (текстовый список с полосой прокрутки)
  - DialogBox (модальное всплывающее окно)
  - CatBanner (фирменный диалоговый виджет с котом из system_os)
*/

#pragma once

#include <Arduino.h>
#include "sys.h"
#include "sys_gfx.h"
#include "sys_xbmp.h"
#include "sys_map_xbmp.h"

class UIElement
{
protected:
  int x, y, w, h;
  bool visible = true;

public:
  UIElement(int x, int y, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
  virtual ~UIElement() {}

  void setPosition(int newX, int newY) { x = newX; y = newY; }
  void setVisible(bool isVis) { visible = isVis; }
  bool isVisible() const { return visible; }

  virtual void draw() = 0;
};

/* 1. Текстовая метка */
class Label : public UIElement
{
private:
  String text;
  bool inverted;

public:
  Label(int x, int y, String text, bool inverted = false)
      : UIElement(x, y, text.length() * 6, 10), text(text), inverted(inverted) {}

  void setText(String newText) { text = newText; w = text.length() * 6; }
  void setInverted(bool inv) { inverted = inv; }

  void draw() override
  {
    if (!visible) return;
    u8g2.setFont(u8g2_font_6x10_tr);
    if (inverted)
    {
      u8g2.setDrawColor(1);
      u8g2.drawBox(x - 1, y - 8, w + 2, 10);
      u8g2.setDrawColor(0);
      u8g2.setCursor(x, y);
      u8g2.print(text);
      u8g2.setDrawColor(1);
    }
    else
    {
      u8g2.setCursor(x, y);
      u8g2.print(text);
    }
  }
};

/* 2. Прогресс-бар */
class ProgressBar : public UIElement
{
private:
  int minVal, maxVal, currentVal;
  bool showPercent;

public:
  ProgressBar(int x, int y, int w, int h, int minVal = 0, int maxVal = 100, bool showPercent = false)
      : UIElement(x, y, w, h), minVal(minVal), maxVal(maxVal), currentVal(minVal), showPercent(showPercent) {}

  void setValue(int val) { currentVal = constrain(val, minVal, maxVal); }
  int getValue() const { return currentVal; }

  void draw() override
  {
    if (!visible) return;
    u8g2.drawFrame(x, y, w, h);
    int innerW = w - 4;
    if (innerW > 0 && maxVal > minVal)
    {
      int fillW = map(currentVal, minVal, maxVal, 0, innerW);
      if (fillW > 0) u8g2.drawBox(x + 2, y + 2, fillW, h - 4);
    }
    if (showPercent && maxVal > minVal)
    {
      int pct = map(currentVal, minVal, maxVal, 0, 100);
      u8g2.setFont(u8g2_font_5x7_tr);
      u8g2.setCursor(x + w + 3, y + h - 1);
      u8g2.print(pct);
      u8g2.print("%");
    }
  }
};

/* 3. Кнопка интерфейса */
class ButtonUI : public UIElement
{
private:
  String caption;
  bool focused = false;

public:
  ButtonUI(int x, int y, int w, int h, String caption)
      : UIElement(x, y, w, h), caption(caption) {}

  void setFocus(bool state) { focused = state; }
  bool isFocused() const { return focused; }
  void setCaption(String cap) { caption = cap; }

  void draw() override
  {
    if (!visible) return;
    u8g2.setFont(u8g2_font_6x10_tr);
    int textX = x + (w - caption.length() * 6) / 2;
    int textY = y + (h + 7) / 2;

    if (focused)
    {
      u8g2.drawRBox(x, y, w, h, 2);
      u8g2.setDrawColor(0);
      u8g2.setCursor(textX, textY);
      u8g2.print(caption);
      u8g2.setDrawColor(1);
    }
    else
    {
      u8g2.drawRFrame(x, y, w, h, 2);
      u8g2.setCursor(textX, textY);
      u8g2.print(caption);
    }
  }
};

/* 4. Прокручиваемый список */
class MenuList : public UIElement
{
private:
  const char** items;
  int itemCount;
  int selectedIndex = 0;
  int topVisibleIndex = 0;
  int maxVisibleRows;
  unsigned long lastNavTime = 0;

public:
  MenuList(int x, int y, int w, int h, const char** items, int count)
      : UIElement(x, y, w, h), items(items), itemCount(count)
  {
    maxVisibleRows = max(1, h / 11);
  }

  int getSelectedIndex() const { return selectedIndex; }

  bool update()
  {
    if (itemCount <= 0) return false;
    int moveY = sys.stick0Y();
    if (moveY != 0 && (millis() - lastNavTime > 170))
    {
      lastNavTime = millis();
      selectedIndex = (moveY > 0) ? (selectedIndex + 1) % itemCount : (selectedIndex - 1 + itemCount) % itemCount;
      if (selectedIndex < topVisibleIndex) topVisibleIndex = selectedIndex;
      if (selectedIndex >= topVisibleIndex + maxVisibleRows) topVisibleIndex = selectedIndex - maxVisibleRows + 1;
    }
    return sys.sw0Pressed();
  }

  void draw() override
  {
    if (!visible || itemCount <= 0) return;
    u8g2.setFont(u8g2_font_6x10_tr);
    int rowsToDraw = min(itemCount, maxVisibleRows);

    for (int i = 0; i < rowsToDraw; i++)
    {
      int idx = topVisibleIndex + i;
      int rowY = y + i * 11;
      if (idx == selectedIndex)
      {
        u8g2.drawBox(x, rowY, w - 4, 10);
        u8g2.setDrawColor(0);
        u8g2.setCursor(x + 3, rowY + 8);
        u8g2.print("> ");
        u8g2.print(items[idx]);
        u8g2.setDrawColor(1);
      }
      else
      {
        u8g2.setCursor(x + 3, rowY + 8);
        u8g2.print("  ");
        u8g2.print(items[idx]);
      }
    }

    if (itemCount > maxVisibleRows)
    {
      int barX = x + w - 2;
      u8g2.drawVLine(barX, y, h);
      int thumbH = max(4, (h * maxVisibleRows) / itemCount);
      int thumbY = y + ((h - thumbH) * selectedIndex) / (itemCount - 1);
      u8g2.drawBox(barX - 1, thumbY, 3, thumbH);
    }
  }
};

/* 5. Всплывающее модальное окно */
class DialogBox : public UIElement
{
private:
  String title, message, buttonText;

public:
  DialogBox(String title, String message, String buttonText = "OK [SW0]")
      : UIElement(10, 8, 108, 44), title(title), message(message), buttonText(buttonText) {}

  void setContent(String newTitle, String newMsg, String newBtn = "OK [SW0]")
  {
    title = newTitle; message = newMsg; buttonText = newBtn;
  }

  void draw() override
  {
    if (!visible) return;
    u8g2.setDrawColor(0);
    u8g2.drawBox(x, y, w, h);
    u8g2.setDrawColor(1);
    u8g2.drawFrame(x, y, w, h);

    u8g2.drawBox(x, y, w, 10);
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(x + 3, y + 8);
    u8g2.print(title);
    u8g2.setDrawColor(1);

    u8g2.setCursor(x + 4, y + 22);
    u8g2.print(message);

    int btnW = buttonText.length() * 6 + 6;
    int btnX = x + (w - btnW) / 2;
    u8g2.drawRBox(btnX, y + h - 13, btnW, 11, 2);
    u8g2.setDrawColor(0);
    u8g2.setCursor(btnX + 3, y + h - 4);
    u8g2.print(buttonText);
    u8g2.setDrawColor(1);
  }
};

/* 6. Фирменный виджет-приветствие с котом и речевым пузырем (фирменный виджет makegame) */
class CatDialogWidget : public UIElement
{
private:
  String line1, line2, line3, hint;
  unsigned long blinkTimer = 0;
  bool showHint = true;

public:
  CatDialogWidget(String l1, String l2, String l3, String hintText = "press SW0")
      : UIElement(0, 0, 128, 60), line1(l1), line2(l2), line3(l3), hint(hintText) {}

  void setLines(String l1, String l2, String l3, String hintText = "press SW0")
  {
    line1 = l1; line2 = l2; line3 = l3; hint = hintText;
  }

  void draw() override
  {
    if (!visible) return;
    u8g2.drawXBMP(2, 2, diaFrameHello_w, diaFrameHello_h, diaFrameHello);
    u8g2.drawXBMP(105, 44, mg_cat_w, mg_cat_h, mg_cat);
    u8g2.drawHLine(102, 58, 21);

    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(9, 13); u8g2.print(line1);
    u8g2.setCursor(9, 23); u8g2.print(line2);
    u8g2.setCursor(9, 33); u8g2.print(line3);

    if (millis() - blinkTimer >= 350)
    {
      blinkTimer = millis();
      showHint = !showHint;
    }
    if (showHint)
    {
      u8g2.setCursor(16, 52);
      u8g2.print(hint);
    }
  }
};
