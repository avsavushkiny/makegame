/* Игровые UI-компоненты для монохромного дисплея 128x60. */
#pragma once
#include <Arduino.h>
#include "sys_ui.h"

class HealthBar : public UIElement {
  uint8_t value, maximum;
  bool showText;
public:
  HealthBar(int x, int y, int w, int h, uint8_t maxValue = 100, bool text = false)
    : UIElement(x,y,w,h), value(maxValue), maximum(maxValue ? maxValue : 1), showText(text) {}
  void setValue(uint8_t v) { value = min(v, maximum); }
  uint8_t getValue() const { return value; }
  void setMaximum(uint8_t m) { maximum = m ? m : 1; value = min(value, maximum); }
  void draw() override {
    if (!visible) return;
    u8g2.drawFrame(x,y,w,h);
    int inner = max(0,w-4);
    int fill = ((long)inner * value) / maximum;
    if (fill > 0 && h > 4) u8g2.drawBox(x+2,y+2,fill,h-4);
    if (showText) { u8g2.setFont(u8g2_font_5x7_tr); u8g2.setCursor(x+w+2,y+h-1); u8g2.print(value); u8g2.print('/'); u8g2.print(maximum); }
  }
};

class LivesCounter : public UIElement {
  uint8_t lives;
public:
  LivesCounter(int x,int y,uint8_t count=3):UIElement(x,y,30,10),lives(count){}
  void setLives(uint8_t n){lives=n;} uint8_t getLives() const{return lives;}
  void draw() override { if(!visible)return; u8g2.setFont(u8g2_font_6x10_tr); u8g2.setCursor(x,y+8); u8g2.print("x"); u8g2.print(lives); }
};

class ScoreCounter : public UIElement {
  int32_t score;
  uint8_t digits;
public:
  ScoreCounter(int x,int y,uint8_t widthDigits=6):UIElement(x,y,widthDigits*6,10),score(0),digits(widthDigits){}
  void setScore(int32_t v){score=max((int32_t)0,v);} void add(int32_t v){setScore(score+v);} int32_t getScore()const{return score;}
  void draw() override { if(!visible)return; u8g2.setFont(u8g2_font_6x10_tr); u8g2.setCursor(x,y+8); u8g2.print(score); }
};

class InventorySlots : public UIElement {
public:
  static const uint8_t MAX_SLOTS=8;
private:
  uint8_t items[MAX_SLOTS]; uint8_t selected=0; uint8_t count;
public:
  InventorySlots(int x,int y,uint8_t slots=4):UIElement(x,y,slots*13,13),count(constrain(slots,1,MAX_SLOTS)){for(uint8_t i=0;i<MAX_SLOTS;i++)items[i]=0;}
  bool setItem(uint8_t slot,uint8_t item){if(slot>=count)return false;items[slot]=item;return true;}
  uint8_t getItem(uint8_t slot)const{return slot<count?items[slot]:0;}
  void select(uint8_t slot){if(slot<count)selected=slot;} uint8_t selectedSlot()const{return selected;}
  void draw() override {if(!visible)return;for(uint8_t i=0;i<count;i++){int sx=x+i*13; if(i==selected)u8g2.drawFrame(sx,y,12,12);else u8g2.drawRFrame(sx,y,12,12,2);if(items[i]){u8g2.setFont(u8g2_font_5x7_tr);u8g2.setCursor(sx+3,y+9);u8g2.print(items[i]);}}}
};
