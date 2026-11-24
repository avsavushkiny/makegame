/*
  sys_engine.h — дополнительные подсистемы makegame OS
  Совместимо с Arduino RP2040 и экраном U8g2 128x60.
  Подсистемы не выделяют память динамически и рассчитаны на небольшую RAM.
#include <math.h>
*/
#pragma once

#include <Arduino.h>
#include "sys.h"
#include "sys_gfx.h"
#include "sys_game_obj.h"

// -----------------------------------------------------------------------------
// 1. Фиксированный шаг обновления физики. Рисование остаётся в основном loop().
// -----------------------------------------------------------------------------
class EngineClock
{
private:
  uint32_t previousMs = 0;
  uint32_t accumulator = 0;
  uint16_t stepMs;
  uint8_t maxSteps;
  uint32_t frames = 0;
  uint32_t fpsStart = 0;
  uint16_t fpsValue = 0;
public:
  EngineClock(uint16_t updateRate = 25, uint8_t maxCatchUp = 4)
    : stepMs(updateRate == 0 ? 40 : updateRate), maxSteps(maxCatchUp) {}

  void begin() { previousMs = millis(); accumulator = 0; fpsStart = previousMs; frames = 0; }
  uint8_t update(void (*fixedUpdate)())
  {
    uint32_t now = millis();
    uint32_t elapsed = now - previousMs;
    previousMs = now;
    if (elapsed > stepMs * maxSteps) elapsed = stepMs * maxSteps;
    accumulator += elapsed;
    uint8_t count = 0;
    while (accumulator >= stepMs && count < maxSteps)
    {
      fixedUpdate();
      accumulator -= stepMs;
      count++;
    }
    frames++;
    if (now - fpsStart >= 1000)
    {
      fpsValue = frames;
      frames = 0;
      fpsStart = now;
    }
    return count;
  }
  uint16_t fps() const { return fpsValue; }
  float interpolation() const { return (float)accumulator / stepMs; }
  uint16_t step() const { return stepMs; }
};

// -----------------------------------------------------------------------------
// 2. Камера 2D: мировые координаты преобразуются в координаты экрана.
// -----------------------------------------------------------------------------
class Camera2D
{
private:
  float x = 0, y = 0;
  int viewW = MG_SCREEN_W, viewH = MG_SCREEN_H;
public:
  Camera2D(int width = MG_SCREEN_W, int height = MG_SCREEN_H) : viewW(width), viewH(height) {}
  void setPosition(float px, float py) { x = px; y = py; }
  void move(float dx, float dy) { x += dx; y += dy; }
  void follow(const GameObject& target)
  {
    x = target.getX() + target.getW() / 2.0f - viewW / 2.0f;
    y = target.getY() + target.getH() / 2.0f - viewH / 2.0f;
  }
  void follow(const GameObject& target, int worldW, int worldH)
  {
    follow(target);
    x = constrain(x, 0, max(0, worldW - viewW));
    y = constrain(y, 0, max(0, worldH - viewH));
  }
  int screenX(float worldX) const { return (int)(worldX - x); }
  int screenY(float worldY) const { return (int)(worldY - y); }
  float worldX(int screenXValue) const { return screenXValue + x; }
  float worldY(int screenYValue) const { return screenYValue + y; }
  bool isVisible(const GameObject& obj) const
  {
    return obj.getX() + obj.getW() > x && obj.getX() < x + viewW &&
           obj.getY() + obj.getH() > y && obj.getY() < y + viewH;
  }
  void draw(const GameObject& obj) const
  {
    if (!obj.isActive() || !isVisible(obj)) return;
    u8g2.drawBox(screenX(obj.getX()), screenY(obj.getY()), obj.getW(), obj.getH());
  }
};

// -----------------------------------------------------------------------------
// 3. Анимация спрайта: массив кадров XBM одинакового размера.
// -----------------------------------------------------------------------------
class SpriteAnimation
{
private:
  const uint8_t* const* frames;
  uint8_t frameCount;
  uint8_t currentFrame = 0;
  uint16_t frameDuration;
  uint32_t lastFrameMs = 0;
  bool looping;
  bool playing = true;
public:
  SpriteAnimation(const uint8_t* const* frameData, uint8_t count,
                 uint16_t durationMs = 120, bool loop = true)
    : frames(frameData), frameCount(count), frameDuration(durationMs), looping(loop) {}
  void reset() { currentFrame = 0; lastFrameMs = millis(); playing = true; }
  void play() { playing = true; lastFrameMs = millis(); }
  void pause() { playing = false; }
  bool isPlaying() const { return playing; }
  uint8_t getFrame() const { return currentFrame; }
  void update()
  {
    if (!playing || frames == nullptr || frameCount == 0) return;
    uint32_t now = millis();
    if (now - lastFrameMs < frameDuration) return;
    lastFrameMs = now;
    if (currentFrame + 1 < frameCount) currentFrame++;
    else if (looping) currentFrame = 0;
    else playing = false;
  }
  void draw(int x, int y, int w, int h) const
  {
    if (frames != nullptr && frameCount > 0 && frames[currentFrame] != nullptr)
      u8g2.drawXBMP(x, y, w, h, frames[currentFrame]);
  }
};

// -----------------------------------------------------------------------------
// 4. Частицы: фиксированный пул эффектов без malloc/new.
// -----------------------------------------------------------------------------
class ParticleSystem
{
public:
  struct Particle { float x, y, vx, vy; uint8_t life; bool active; };
private:
  Particle* particles;
  uint8_t capacity;
  uint8_t cursor = 0;
public:
  ParticleSystem(Particle* storage, uint8_t count) : particles(storage), capacity(count) {}
  void clear()
  {
    for (uint8_t i = 0; i < capacity; i++) particles[i].active = false;
  }
  void emit(float x, float y, uint8_t amount = 6, uint8_t life = 18)
  {
    if (particles == nullptr || capacity == 0) return;
    for (uint8_t n = 0; n < amount; n++)
    {
      Particle& p = particles[cursor];
      cursor = (cursor + 1) % capacity;
      float angle = random(0, 628) / 100.0f;
      float speed = random(5, 20) / 10.0f;
      p.x = x; p.y = y;
      p.vx = cos(angle) * speed;
      p.vy = sin(angle) * speed;
      p.life = life; p.active = true;
    }
  }
  void update()
  {
    if (particles == nullptr) return;
    for (uint8_t i = 0; i < capacity; i++)
    {
      Particle& p = particles[i];
      if (!p.active) continue;
      p.x += p.vx; p.y += p.vy; p.vy += 0.08f;
      if (p.life > 0) p.life--;
      if (p.life == 0) p.active = false;
    }
  }
  void draw() const
  {
    if (particles == nullptr) return;
    for (uint8_t i = 0; i < capacity; i++)
      if (particles[i].active) u8g2.drawPixel((int)particles[i].x, (int)particles[i].y);
  }
};

// -----------------------------------------------------------------------------
// 5. Менеджер игровых состояний. Упрощает меню, игру, паузу и экран результата.
// -----------------------------------------------------------------------------
enum EngineState : uint8_t { ENGINE_MENU, ENGINE_PLAYING, ENGINE_PAUSED, ENGINE_GAME_OVER };

class GameStateManager
{
private:
  EngineState state = ENGINE_MENU;
  void (*onEnter[4])() = { nullptr, nullptr, nullptr, nullptr };
  void (*onUpdate[4])() = { nullptr, nullptr, nullptr, nullptr };
  void (*onDraw[4])() = { nullptr, nullptr, nullptr, nullptr };
public:
  void configure(EngineState s, void (*enterFn)(), void (*updateFn)(), void (*drawFn)())
  {
    uint8_t i = (uint8_t)s;
    if (i > 3) return;
    onEnter[i] = enterFn; onUpdate[i] = updateFn; onDraw[i] = drawFn;
  }
  void change(EngineState next)
  {
    if (state == next) return;
    state = next;
    if (onEnter[(uint8_t)state] != nullptr) onEnter[(uint8_t)state]();
  }
  void update() { if (onUpdate[(uint8_t)state] != nullptr) onUpdate[(uint8_t)state](); }
  void draw() { if (onDraw[(uint8_t)state] != nullptr) onDraw[(uint8_t)state](); }
  EngineState getState() const { return state; }
  bool is(EngineState s) const { return state == s; }
};

// -----------------------------------------------------------------------------
// 6. Вспомогательные функции столкновений и движения.
// -----------------------------------------------------------------------------
inline bool mgPointInRect(int px, int py, int x, int y, int w, int h)
{
  return px >= x && px < x + w && py >= y && py < y + h;
}

inline bool mgCircleHit(float ax, float ay, float ar, float bx, float by, float br)
{
  float dx = ax - bx, dy = ay - by;
  float radius = ar + br;
  return dx * dx + dy * dy <= radius * radius;
}
