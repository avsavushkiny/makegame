/* Small fixed-size event queue. No dynamic allocation. */
#pragma once
#include <Arduino.h>

enum GameEventType : uint8_t {
  GAME_EVENT_NONE = 0,
  GAME_EVENT_COLLISION,
  GAME_EVENT_DAMAGE,
  GAME_EVENT_DESTROYED,
  GAME_EVENT_ITEM_COLLECTED,
  GAME_EVENT_LEVEL_COMPLETE,
  GAME_EVENT_PLAYER_DIED,
  GAME_EVENT_CUSTOM
};

struct GameEvent {
  GameEventType type;
  int16_t source;
  int16_t target;
  int32_t value;
};

class GameEventSystem
{
public:
  static const uint8_t CAPACITY = 16;
  typedef void (*EventCallback)(const GameEvent& event);
private:
  GameEvent queue[CAPACITY];
  uint8_t head = 0, tail = 0, count = 0;
  EventCallback listener = nullptr;
  uint16_t dropped = 0;
public:
  void setListener(EventCallback callback) { listener = callback; }
  bool post(GameEventType type, int16_t source = -1, int16_t target = -1, int32_t value = 0) {
    if (type == GAME_EVENT_NONE || count >= CAPACITY) { if (count >= CAPACITY) ++dropped; return false; }
    queue[tail] = { type, source, target, value };
    tail = (tail + 1) % CAPACITY;
    ++count;
    return true;
  }
  bool poll(GameEvent& event) {
    if (!count) return false;
    event = queue[head];
    head = (head + 1) % CAPACITY;
    --count;
    return true;
  }
  uint8_t dispatch(uint8_t limit = CAPACITY) {
    if (!listener) return 0;
    GameEvent event;
    uint8_t processed = 0;
    while (processed < limit && poll(event)) { listener(event); ++processed; }
    return processed;
  }
  void clear() { head = tail = count = 0; }
  uint8_t pending() const { return count; }
  uint16_t droppedEvents() const { return dropped; }
};
