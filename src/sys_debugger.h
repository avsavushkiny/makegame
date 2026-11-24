/* Runtime diagnostics overlay for makegame OS. */
#pragma once
#include <Arduino.h>
#include "sys_gfx.h"
#include "sys_object_manager.h"

class GameDebugger {
private:
  bool enabled=false;
  uint32_t lastSample=0;
  uint16_t frames=0, fps=0;
  uint16_t updateUs=0;
  uint16_t collisions=0;
  uint8_t objectCount=0;
  uint32_t updateStartedAt=0;
  bool updateTiming=false;
public:
  void enable(bool state=true) { enabled=state; }
  void toggle() { enabled=!enabled; }
  bool isEnabled() const { return enabled; }
  void begin() { lastSample=millis(); frames=0; fps=0; }
  void frame() {
    ++frames; uint32_t now=millis();
    if (now-lastSample>=1000) { fps=frames; frames=0; lastSample=now; }
  }
  void beginUpdate() { updateStartedAt=micros(); updateTiming=true; }
  void endUpdate() { if (updateTiming) { uint32_t elapsed=micros()-updateStartedAt; updateUs=(uint16_t)min(elapsed,(uint32_t)65535); updateTiming=false; } }
  void setUpdateTimeMicros(uint16_t value) { updateUs=value; }
  void setCollisionCount(uint16_t value) { collisions=value; }
  void setObjectCount(uint8_t value) { objectCount=value; }
  template <typename Manager> void sampleObjects(const Manager& manager) { objectCount=manager.size(); }
  void draw(int16_t x=1,int16_t y=1) const {
    if (!enabled) return;
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.setDrawColor(0); u8g2.drawBox(x,y,75,34); u8g2.setDrawColor(1);
    char line[24];
    snprintf(line,sizeof(line),"FPS:%u",(unsigned)fps); u8g2.drawStr(x+2,y+7,line);
    snprintf(line,sizeof(line),"OBJ:%u COL:%u",(unsigned)objectCount,(unsigned)collisions); u8g2.drawStr(x+2,y+15,line);
    snprintf(line,sizeof(line),"UPD:%uus",(unsigned)updateUs); u8g2.drawStr(x+2,y+23,line);
    u8g2.drawStr(x+2,y+31,"DEBUG ON");
  }
};
