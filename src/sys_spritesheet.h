/* SpriteSheetManager: fixed-memory registry of XBM frame sets for RP2040. */
#pragma once
#include <Arduino.h>
#include "sys_gfx.h"

class SpriteSheetManager {
public:
  static const uint8_t MAX_SHEETS = 12;
  struct Sheet {
    const uint8_t* const* frames;
    uint8_t frameCount;
    uint8_t frameWidth, frameHeight;
  };
private:
  Sheet sheets[MAX_SHEETS];
  uint8_t count = 0;
public:
  SpriteSheetManager() {
    for (uint8_t i = 0; i < MAX_SHEETS; ++i)
      sheets[i] = { nullptr, 0, 0, 0 };
  }
  int8_t add(const uint8_t* const* frames, uint8_t frameCount,
             uint8_t width, uint8_t height) {
    if (!frames || !frameCount || !width || !height || count >= MAX_SHEETS) return -1;
    for (uint8_t i = 0; i < frameCount; ++i) if (!frames[i]) return -1;
    sheets[count] = { frames, frameCount, width, height };
    return (int8_t)count++;
  }
  const Sheet* get(uint8_t id) const { return id < count ? &sheets[id] : nullptr; }
  const uint8_t* frame(uint8_t id, uint8_t frameIndex) const {
    const Sheet* s = get(id);
    return (s && frameIndex < s->frameCount) ? s->frames[frameIndex] : nullptr;
  }
  uint8_t size() const { return count; }
  bool draw(uint8_t id, uint8_t frameIndex, int16_t x, int16_t y) const {
    const Sheet* s = get(id);
    if (!s || frameIndex >= s->frameCount) return false;
    u8g2.drawXBMP(x, y, s->frameWidth, s->frameHeight, s->frames[frameIndex]);
    return true;
  }
};
