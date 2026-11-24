/*
  sys_game_obj.h — Игровой микро-движок (ООП) для консоли RP2040 (128x60)
*/

#pragma once

#include <Arduino.h>
#include "sys.h"
#include "sys_gfx.h"
#include "sys_sound.h"

class GameObject
{
protected:
  float x, y;
  int w, h;
  float vx = 0, vy = 0;
  bool active = true;

public:
  GameObject(float x, float y, int w, int h) : x(x), y(y), w(w), h(h) {}
  virtual ~GameObject() {}

  float getX() const { return x; }
  float getY() const { return y; }
  int getW() const { return w; }
  int getH() const { return h; }
  bool isActive() const { return active; }
  float getVelocityX() const { return vx; }
  float getVelocityY() const { return vy; }
  void setSize(int newW, int newH) { w = max(1, newW); h = max(1, newH); }
  void setVelocityX(float value) { vx = value; }
  void setVelocityY(float value) { vy = value; }

  void setPosition(float newX, float newY) { x = newX; y = newY; }
  void setVelocity(float newVx, float newVy) { vx = newVx; vy = newVy; }
  void setActive(bool state) { active = state; }

  virtual void update()
  {
    if (!active) return;
    x += vx;
    y += vy;
  }

  virtual void draw()
  {
    if (!active) return;
    u8g2.drawBox((int)x, (int)y, w, h);
  }

  bool collidesWith(const GameObject& other) const
  {
    if (!active || !other.active) return false;
    return (x < other.x + other.w) &&
           (x + w > other.x) &&
           (y < other.y + other.h) &&
           (y + h > other.y);
  }

  bool containsPoint(int px, int py) const
  {
    return active && px >= x && px < x + w && py >= y && py < y + h;
  }

  void clampToScreen()
  {
    x = constrain(x, 0, MG_SCREEN_W - w);
    y = constrain(y, 0, MG_SCREEN_H - h);
  }

  bool isOutOfScreen() const
  {
    return (x + w < 0) || (x > MG_SCREEN_W) || (y + h < 0) || (y > MG_SCREEN_H);
  }
};

class SpriteObject : public GameObject
{
protected:
  const uint8_t* bitmap;

public:
  SpriteObject(float x, float y, int w, int h, const uint8_t* xbmData)
      : GameObject(x, y, w, h), bitmap(xbmData) {}

  void setSprite(const uint8_t* newBitmap) { bitmap = newBitmap; }

  void draw() override
  {
    if (!active || bitmap == nullptr) return;
    u8g2.drawXBMP((int)x, (int)y, w, h, bitmap);
  }
};

class PlayerActor : public GameObject
{
private:
  float speed;
  int hp, maxHp;
  bool useRightStick;
  const uint8_t* sprite;

public:
  PlayerActor(float x, float y, int w, int h, float speed = 1.5, int maxHp = 3, bool rightStick = false, const uint8_t* xbm = nullptr)
      : GameObject(x, y, w, h), speed(speed), hp(maxHp), maxHp(maxHp), useRightStick(rightStick), sprite(xbm) {}

  int getHp() const { return hp; }
  int getMaxHp() const { return maxHp; }

  void takeDamage(int dmg = 1)
  {
    hp = max(0, hp - dmg);
    sound.play(SND_HIT);
    if (hp == 0) active = false;
  }

  void heal(int amount = 1)
  {
    hp = min(maxHp, hp + amount);
    if (hp > 0) active = true;
  }

  void reset(float startX, float startY)
  {
    x = startX; y = startY; hp = maxHp; active = true;
  }

  void update() override
  {
    if (!active) return;
    int dx = useRightStick ? sys.stick1X() : sys.stick0X();
    int dy = useRightStick ? sys.stick1Y() : sys.stick0Y();
    x = constrain(x + dx * speed, 0, MG_SCREEN_W - w);
    y = constrain(y + dy * speed, 0, MG_SCREEN_H - h);
  }

  void draw() override
  {
    if (!active) return;
    if (sprite != nullptr)
      u8g2.drawXBMP((int)x, (int)y, w, h, sprite);
    else
    {
      u8g2.drawFrame((int)x, (int)y, w, h);
      u8g2.drawPixel((int)x + w / 2, (int)y + h / 2);
    }
  }
};

class Projectile : public GameObject
{
public:
  Projectile() : GameObject(0, 0, 3, 2) { active = false; }

  void fire(float startX, float startY, float velX, float velY)
  {
    x = startX; y = startY; vx = velX; vy = velY; active = true;
    sound.play(SND_LASER);
  }

  void update() override
  {
    if (!active) return;
    GameObject::update();
    if (isOutOfScreen()) active = false;
  }
};

class FallingHazard : public GameObject
{
private:
  bool autoRespawnTop;

public:
  FallingHazard(int w = 6, int h = 6, bool autoRespawn = true)
      : GameObject(0, 0, w, h), autoRespawnTop(autoRespawn) { respawn(); }

  void respawn()
  {
    x = random(0, MG_SCREEN_W - w);
    y = -random(5, 45);
    vy = random(8, 22) / 10.0f;
    vx = 0;
    active = true;
  }

  void update() override
  {
    if (!active) return;
    GameObject::update();
    if (y > MG_SCREEN_H && autoRespawnTop) respawn();
  }

  void draw() override
  {
    if (!active) return;
    u8g2.drawDisc((int)x + w / 2, (int)y + h / 2, w / 2);
  }
};
