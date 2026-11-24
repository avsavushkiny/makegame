/* Fixed-capacity object registry. Objects remain owned by the game. */
#pragma once
#include <Arduino.h>
#include "sys_game_obj.h"
#include "sys_engine.h"

class GameObjectManager
{
private:
  GameObject** items;
  uint8_t capacity, count = 0;
public:
  GameObjectManager(GameObject** storage, uint8_t maxObjects) : items(storage), capacity(maxObjects) {}
  bool add(GameObject& object) {
    if (!items || count >= capacity) return false;
    for (uint8_t i = 0; i < count; ++i) if (items[i] == &object) return true;
    items[count++] = &object; return true;
  }
  bool remove(GameObject& object) {
    for (uint8_t i = 0; i < count; ++i) if (items[i] == &object) {
      for (uint8_t j = i + 1; j < count; ++j) items[j - 1] = items[j];
      --count; return true;
    }
    return false;
  }
  void clear() { count = 0; }
  uint8_t size() const { return count; }
  GameObject* at(uint8_t index) const { return index < count ? items[index] : nullptr; }
  void update() { for (uint8_t i = 0; i < count; ++i) if (items[i]) items[i]->update(); }
  void draw() { for (uint8_t i = 0; i < count; ++i) if (items[i]) items[i]->draw(); }
  void draw(const Camera2D& camera) {
    for (uint8_t i = 0; i < count; ++i) if (items[i] && items[i]->isActive() && camera.isVisible(*items[i])) {
      GameObject* o = items[i];
      u8g2.drawBox(camera.screenX(o->getX()), camera.screenY(o->getY()), o->getW(), o->getH());
    }
  }
  void removeInactive() {
    for (uint8_t i = 0; i < count;) {
      if (!items[i] || !items[i]->isActive()) {
        for (uint8_t j = i + 1; j < count; ++j) items[j - 1] = items[j];
        --count;
      } else ++i;
    }
  }
  void checkCollisions(void (*callback)(GameObject&, GameObject&)) {
    if (!callback) return;
    for (uint8_t i = 0; i < count; ++i) for (uint8_t j = i + 1; j < count; ++j)
      if (items[i] && items[j] && items[i]->collidesWith(*items[j])) callback(*items[i], *items[j]);
  }
};
