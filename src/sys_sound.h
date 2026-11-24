/*
  sys_sound.h — Звуковой движок на втором ядре (Core 1) процессора RP2040
  Пин пьезоизлучателя (Buzzer): 9
  Позволяет проигрывать мелодии и звуковые эффекты в фоне без торможения графики на Core 0!
*/

#pragma once

#include <Arduino.h>

#define PIN_BUZZER 9

struct Note
{
  int freq;     // Частота в Гц (0 = пауза)
  int duration; // Длительность в мс
};

// Идентификаторы системных и игровых мелодий
enum SoundTrackId
{
  SND_NONE = 0,
  SND_BOOT,     // Стартовая мелодия консоли
  SND_MENU_MOVE,    // Короткий щелчок при перелистывании меню
  SND_MENU_SELECT,  // Звук запуска игры
  SND_LASER,        // Выстрел в игре
  SND_HIT,          // Попадание / отскок мяча
  SND_COIN,         // Подбор бонуса / очко
  SND_GAME_OVER     // Мелодия проигрыша
};

// Встроенные мелодии
inline const Note MELODY_BOOT[] = {
  {330, 120}, {2637, 120}, {415, 120}, {523, 100}, {494, 150}, {1865, 120}, {2489, 220}
};
inline const Note MELODY_MENU_MOVE[]   = { {880, 25} };
inline const Note MELODY_MENU_SELECT[] = { {523, 60}, {659, 60}, {784, 110} };
inline const Note MELODY_LASER[]       = { {1400, 25}, {900, 30} };
inline const Note MELODY_HIT[]         = { {220, 40}, {140, 50} };
inline const Note MELODY_COIN[]        = { {988, 60}, {1319, 140} };
inline const Note MELODY_GAME_OVER[]   = { {392, 160}, {349, 160}, {330, 160}, {262, 320} };

class SoundEngineCore1
{
private:
  volatile SoundTrackId requestedTrack = SND_NONE;
  volatile bool stopFlag = false;
  bool muted = false;

  void playSequence(const Note* notes, int count)
  {
    for (int i = 0; i < count; i++)
    {
      if (stopFlag || requestedTrack != SND_NONE) break;
      if (notes[i].freq > 0)
      {
        tone(PIN_BUZZER, notes[i].freq);
      }
      else
      {
        noTone(PIN_BUZZER);
      }
      delay(notes[i].duration);
      noTone(PIN_BUZZER);
      delay(15); // Микропауза между нотами для четкости артикуляции
    }
    noTone(PIN_BUZZER);
  }

public:
  // Вызывается в setup1() на втором ядре
  void beginCore1()
  {
    pinMode(PIN_BUZZER, OUTPUT);
    noTone(PIN_BUZZER);
  }

  // Вызывается с первого ядра (Core 0) — мгновенно ставит трек в очередь без блокировки игры
  void play(SoundTrackId id)
  {
    if (muted) return;
    stopFlag = true;
    requestedTrack = id;
  }

  void stop()
  {
    stopFlag = true;
    requestedTrack = SND_NONE;
  }

  void setMuted(bool state)
  {
    muted = state;
    if (muted) stop();
  }

  bool isMuted() const { return muted; }

  // Основной цикл второго ядра — вызывается в loop1()
  void updateCore1()
  {
    if (requestedTrack == SND_NONE)
    {
      delay(5);
      return;
    }

    SoundTrackId current = requestedTrack;
    requestedTrack = SND_NONE;
    stopFlag = false;

    if (muted) return;

    switch (current)
    {
      case SND_BOOT:
        playSequence(MELODY_BOOT, sizeof(MELODY_BOOT) / sizeof(Note));
        break;
      case SND_MENU_MOVE:
        playSequence(MELODY_MENU_MOVE, sizeof(MELODY_MENU_MOVE) / sizeof(Note));
        break;
      case SND_MENU_SELECT:
        playSequence(MELODY_MENU_SELECT, sizeof(MELODY_MENU_SELECT) / sizeof(Note));
        break;
      case SND_LASER:
        playSequence(MELODY_LASER, sizeof(MELODY_LASER) / sizeof(Note));
        break;
      case SND_HIT:
        playSequence(MELODY_HIT, sizeof(MELODY_HIT) / sizeof(Note));
        break;
      case SND_COIN:
        playSequence(MELODY_COIN, sizeof(MELODY_COIN) / sizeof(Note));
        break;
      case SND_GAME_OVER:
        playSequence(MELODY_GAME_OVER, sizeof(MELODY_GAME_OVER) / sizeof(Note));
        break;
      default:
        break;
    }
  }
};

inline SoundEngineCore1 sound;
