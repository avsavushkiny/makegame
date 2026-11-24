/* SceneManager: fixed-capacity scene lifecycle manager for makegame OS. */
#pragma once
#include <Arduino.h>

class SceneManager
{
public:
  static const uint8_t MAX_SCENES = 8;
  typedef void (*SceneCallback)();
  struct Scene {
    SceneCallback onEnter;
    SceneCallback onExit;
    SceneCallback onUpdate;
    SceneCallback onDraw;
  };
private:
  Scene scenes[MAX_SCENES];
  uint8_t sceneCount = 0;
  int8_t current = -1;
  int8_t pending = -1;
  bool transitioning = false;
  void applyTransition() {
    if (pending < 0 || pending >= (int8_t)sceneCount) return;
    int8_t next = pending;
    pending = -1;
    transitioning = true;
    if (current >= 0 && scenes[current].onExit) scenes[current].onExit();
    current = next;
    if (scenes[current].onEnter) scenes[current].onEnter();
    transitioning = false;
  }
public:
  SceneManager() {
    for (uint8_t i = 0; i < MAX_SCENES; ++i)
      scenes[i] = { nullptr, nullptr, nullptr, nullptr };
  }
  int8_t addScene(SceneCallback enterFn, SceneCallback exitFn,
                  SceneCallback updateFn, SceneCallback drawFn) {
    if (sceneCount >= MAX_SCENES) return -1;
    scenes[sceneCount] = { enterFn, exitFn, updateFn, drawFn };
    return (int8_t)sceneCount++;
  }
  bool changeTo(uint8_t sceneId) {
    if (sceneId >= sceneCount) return false;
    pending = (int8_t)sceneId;
    if (!transitioning) applyTransition();
    return true;
  }
  void update() {
    transitioning = true;
    if (current >= 0 && scenes[current].onUpdate) scenes[current].onUpdate();
    transitioning = false;
    applyTransition();
  }
  void draw() {
    if (current >= 0 && scenes[current].onDraw) scenes[current].onDraw();
  }
  int8_t currentScene() const { return current; }
  bool hasScene() const { return current >= 0; }
};
