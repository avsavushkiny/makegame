/* Lightweight axis-separated platform physics. */
#pragma once
#include <Arduino.h>
#include "sys_game_obj.h"
#include "sys_tilemap.h"

class PhysicsBody
{
private:
  GameObject* object;
  float gravity, friction, bounce, maxFallSpeed;
  bool grounded = false, useGravity = true;
public:
  PhysicsBody(GameObject& target, float gravityValue = 0.18f, float frictionValue = 0.82f)
    : object(&target), gravity(gravityValue), friction(frictionValue), bounce(0), maxFallSpeed(3.5f) {}
  void setGravity(float value) { gravity = value; }
  void setFriction(float value) { friction = constrain(value, 0.0f, 1.0f); }
  void setBounce(float value) { bounce = max(0.0f, value); }
  void setMaxFallSpeed(float value) { maxFallSpeed = max(0.1f, value); }
  void enableGravity(bool enabled) { useGravity = enabled; }
  bool isGrounded() const { return grounded; }
  void jump(float velocity) { if (grounded) { object->setVelocityY(-abs(velocity)); grounded = false; } }

  void update(const TileMap& map) {
    if (!object || !object->isActive()) return;
    float vx = object->getVelocityX(), vy = object->getVelocityY();
    if (useGravity) vy = min(maxFallSpeed, vy + gravity);
    float nx = object->getX() + vx;
    if (!map.overlapsSolid(nx, object->getY(), object->getW(), object->getH())) object->setPosition(nx, object->getY());
    else { vx = 0; object->setVelocityX(0); }

    float ny = object->getY() + vy;
    grounded = false;
    if (!map.overlapsSolid(object->getX(), ny, object->getW(), object->getH())) object->setPosition(object->getX(), ny);
    else {
      if (vy > 0) grounded = true;
      vy = (bounce > 0 && abs(vy) > 0.5f) ? -abs(vy) * bounce : 0;
      object->setVelocityY(vy);
    }
    vx *= friction;
    if (abs(vx) < 0.03f) vx = 0;
    object->setVelocityX(vx);
  }
};
