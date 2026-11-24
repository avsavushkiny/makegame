/*
  sys_audio_manager.h — единая точка управления звуком makegame OS.
  Аппаратный движок sys_sound.h остаётся одноголосым: одновременно звучит один эффект.
  Менеджер добавляет приоритеты, master volume (логический уровень) и mute.
*/
#pragma once
#include <Arduino.h>
#include "sys_sound.h"

class GameAudioManager {
public:
  enum Priority : uint8_t { PRIORITY_LOW = 0, PRIORITY_NORMAL = 1, PRIORITY_HIGH = 2, PRIORITY_CRITICAL = 3 };
private:
  uint8_t masterVolume = 100;
  bool muted = false;
  uint8_t activePriority = PRIORITY_LOW;
  uint32_t lastRequestAt = 0;
  uint16_t minIntervalMs = 0;
public:
  void begin() { sound.setMuted(muted || masterVolume == 0); }
  void setMuted(bool value) { muted = value; sound.setMuted(muted || masterVolume == 0); }
  bool isMuted() const { return muted; }
  void setMasterVolume(uint8_t value) { masterVolume = value > 100 ? 100 : value; sound.setMuted(muted || masterVolume == 0); }
  uint8_t getMasterVolume() const { return masterVolume; }
  void setMinimumInterval(uint16_t ms) { minIntervalMs = ms; }
  bool play(SoundTrackId id, Priority priority = PRIORITY_NORMAL) {
    if (id == SND_NONE || muted || masterVolume == 0) return false;
    uint32_t now = millis();
    if (minIntervalMs && now - lastRequestAt < minIntervalMs && priority <= activePriority) return false;
    // SoundEngineCore1 не сообщает о завершении трека, поэтому приоритет применяется
    // к последовательности запросов, а не к фактическому времени звучания.
    sound.play(id);
    activePriority = priority;
    lastRequestAt = now;
    return true;
  }
  void stop() { sound.stop(); activePriority = PRIORITY_LOW; }
  void resetPriority() { activePriority = PRIORITY_LOW; }
};

inline GameAudioManager gameAudio;
