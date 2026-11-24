/* Lightweight page-based dialog system. Text storage remains owned by the caller. */
#pragma once
#include <Arduino.h>
#include "sys_gfx.h"

class GameDialog {
public:
  static const uint8_t MAX_CHOICES = 4;
  typedef void (*ChoiceCallback)(uint8_t choiceIndex);
private:
  const char* const* pages=nullptr;
  uint8_t pageCount=0, page=0;
  const char* const* choices=nullptr;
  uint8_t choiceCount=0, selected=0;
  ChoiceCallback onChoice=nullptr;
  bool visible=false;
public:
  bool open(const char* const* textPages,uint8_t count,
            const char* const* choiceLabels=nullptr,uint8_t choicesCount=0,
            ChoiceCallback callback=nullptr) {
    if (!textPages || !count || choicesCount>MAX_CHOICES || (choicesCount && !choiceLabels)) return false;
    pages=textPages; pageCount=count; page=0; choices=choiceLabels;
    choiceCount=choicesCount; selected=0; onChoice=callback; visible=true; return true;
  }
  void close() { visible=false; pages=nullptr; choices=nullptr; pageCount=choiceCount=page=selected=0; }
  bool isVisible() const { return visible; }
  bool nextPage() { if (!visible || page+1>=pageCount) return false; ++page; return true; }
  bool previousPage() { if (!visible || page==0) return false; --page; return true; }
  void moveChoice(int8_t delta) {
    if (!visible || !choiceCount) return;
    int16_t n=(int16_t)selected+delta;
    while(n<0) n+=choiceCount;
    selected=(uint8_t)(n%choiceCount);
  }
  void confirm() {
    if (!visible) return;
    if (choiceCount) { ChoiceCallback cb=onChoice; uint8_t choice=selected; close(); if(cb) cb(choice); }
    else if (!nextPage()) close();
  }
  void draw(int16_t x=2,int16_t y=2,int16_t width=124,int16_t height=56) const {
    if (!visible || !pages || !pages[page]) return;
    u8g2.drawFrame(x,y,width,height);
    u8g2.setFont(u8g2_font_5x7_tr);
    // Keep complete UTF-8 lines intact for U8g2 drawUTF8().
    int16_t baseline=y+10;
    char line[48]; uint8_t len=0;
    const char* p=pages[page];
    while (*p && baseline < y+height-12-(choiceCount ? choiceCount*8 : 0)) {
      if (*p=='\n' || len>=sizeof(line)-1) {
        line[len]=0; u8g2.drawUTF8(x+4,baseline,line);
        baseline+=8; len=0;
        if (*p=='\n') ++p;
        continue;
      }
      line[len++]=*p++;
    }
    if (len && baseline < y+height-12-(choiceCount ? choiceCount*8 : 0)) { line[len]=0; u8g2.drawUTF8(x+4,baseline,line); }
    if (choiceCount && choices) {
      int16_t cy=y+height-4;
      for(uint8_t i=0;i<choiceCount;++i) {
        if (i) { u8g2.drawStr(x+4,cy," "); }
        if (i==selected) u8g2.drawStr(x+4,cy,">");
        u8g2.drawStr(x+10,cy,choices[i] ? choices[i] : "-");
        cy-=8;
      }
    } else {
      u8g2.drawStr(x+width-28,y+height-4,page+1<pageCount?"NEXT":"OK");
    }
  }
};
