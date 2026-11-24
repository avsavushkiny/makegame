/* AnimationController: compact frame animation for SpriteObject/XBM. */
#pragma once
#include <Arduino.h>
#include "sys_game_obj.h"

class AnimationController
{
public:
  struct Clip {
    const uint8_t* const* frames;
    uint8_t frameCount;
    uint16_t frameDurationMs;
    bool loop;
  };
private:
  SpriteObject* target;
  const Clip* clips;
  uint8_t clipCount;
  int8_t currentClip = -1;
  uint8_t frame = 0;
  uint32_t lastFrameAt = 0;
  bool playing = false;
  void applyFrame() {
    if (!target || currentClip < 0 || currentClip >= (int8_t)clipCount) return;
    const Clip& c = clips[currentClip];
    if (c.frames && c.frameCount && c.frames[frame]) target->setSprite(c.frames[frame]);
  }
public:
  AnimationController(SpriteObject& object, const Clip* definitions, uint8_t count)
    : target(&object), clips(definitions), clipCount(count) {}
  bool play(uint8_t clipId, bool restart = true) {
    if (!clips || clipId >= clipCount || !clips[clipId].frames || clips[clipId].frameCount == 0) return false;
    if (currentClip != (int8_t)clipId || restart) frame = 0;
    currentClip = (int8_t)clipId;
    playing = true;
    lastFrameAt = millis();
    applyFrame();
    return true;
  }
  void stop() { playing = false; }
  void pause() { playing = false; }
  void resume() { if (currentClip >= 0) { playing = true; lastFrameAt = millis(); } }
  bool isPlaying() const { return playing; }
  uint8_t currentFrame() const { return frame; }
  int8_t currentAnimation() const { return currentClip; }
  void update() {
    if (!playing || !clips || currentClip < 0 || currentClip >= (int8_t)clipCount) return;
    const Clip& c = clips[currentClip];
    uint16_t duration = c.frameDurationMs ? c.frameDurationMs : 1;
    uint32_t now = millis();
    if (now - lastFrameAt < duration) return;
    lastFrameAt = now;
    if (frame + 1 < c.frameCount) ++frame;
    else if (c.loop) frame = 0;
    else { playing = false; return; }
    applyFrame();
  }
};
