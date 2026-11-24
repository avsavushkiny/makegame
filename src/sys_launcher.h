/*
  sys_launcher.h — Графическая оболочка makegame OS (объединение system_os + mg_os)
  Содержит:
  - Стартовую заставку (логотип + музыка на Core 1)
  - Приветственный экран с котом (CatDialogWidget) — опционально или по кнопке
  - Главное игровое меню с иконками 48x48, кирпичным полом, котом за ПК и рамкой диалога
  - Автоматический скринсейвер с отключением подсветки при простое (60 сек)
*/

#pragma once

#include <Arduino.h>
#include "sys.h"
#include "sys_gfx.h"
#include "sys_sound.h"
#include "sys_xbmp.h"
#include "sys_map_xbmp.h"
#include "sys_image48.h"
#include "sys_ui.h"

namespace user
{
  struct GameCard
  {
    int id;
    const char* titleLine1;      // Первая строка названия (до 9 символов)
    const char* titleLine2;      // Вторая строка названия (до 9 символов)
    const uint8_t* icon48;       // Иконка 48x48 пикселей из sys_image48.h
    void (*loopFn)();            // Функция игрового цикла
    void (*initFn)() = nullptr;  // Функция инициализации при старте игры
  };
}

class GraphicalOS
{
private:
  user::GameCard* cards = nullptr;
  int cardCount = 0;
  int currentIndex = 0;
  int activeGameIndex = -1; // -1 = главное меню, >= 0 = запущенная игра

  unsigned long lastNavTime = 0;
  unsigned long blinkPlayTimer = 0;
  bool showPlayBadge = true;

  // Переменные скринсейвера
  bool inScreenSaver = false;
  int saverX = 20;
  int saverY = 15;
  unsigned long saverMoveTimer = 0;
  int saverCycles = 0;
  const unsigned long IDLE_TIMEOUT_MS = 60000; // 60 секунд до заставки

  // Отрисовка экрана загрузки игры (Splash)
  void showLaunchSplash(const char* line1, const char* line2)
  {
    sound.play(SND_MENU_SELECT);
    u8g2.clearBuffer();
    u8g2.drawRFrame(6, 10, 116, 40, 3);
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(14, 26);
    u8g2.print("LOADING...");
    u8g2.setCursor(14, 39);
    u8g2.print(line1);
    u8g2.print(" ");
    u8g2.print(line2);
    u8g2.sendBuffer();
    delay(650);
  }

  // Отрисовка красивого меню из system_os, адаптированного под 128x60
  void drawVisualMenu()
  {
    // Верхняя линия и кирпичный пол внизу (y = 51..59, идеально влезает в 60 пикселей!)
    u8g2.drawHLine(0, 0, 128);
    for (int x = 0; x < 128; x += 8)
    {
      u8g2.drawXBMP(x, 51, wall2_w, wall2_h, wall2_bits);
    }

    // Левая карточка с иконкой игры 48x48
    u8g2.drawRFrame(4, 3, 50, 48, 1);
    if (cards[currentIndex].icon48 != nullptr)
    {
      u8g2.drawXBMP(5, 4, 48, 48, cards[currentIndex].icon48);
    }

    // Правая часть: диалоговое облачко, кот и монитор
    u8g2.drawXBMP(59, 2, messageMenu_w, messageMenu_h, messageMenu_bits);
    u8g2.drawXBMP(62, 37, nes_cat_w, nes_cat_h, nes_cat0);
    u8g2.drawXBMP(94, 38, pcCat_w, pcCat_h, pcCat_bits);

    // Анимация "мерцающего экрана" на мониторе кота
    for (int a = 0; a < 14; a++)
    {
      u8g2.drawPixel(random(98, 106), random(40, 44));
    }

    // Счетчик выбранной игры (например, "1/6") и индикатор звука
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.setCursor(63, 11);
    u8g2.print(currentIndex + 1);
    u8g2.print("/");
    u8g2.print(cardCount);

    u8g2.setCursor(102, 11);
    u8g2.print(sound.isMuted() ? "MUTE" : "SND");

    // Название выбранной игры в две строки
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(63, 21);
    u8g2.print(cards[currentIndex].titleLine1);
    u8g2.setCursor(63, 31);
    u8g2.print(cards[currentIndex].titleLine2);

    // Мигающий бейдж PLAY в правом нижнем углу
    if (millis() - blinkPlayTimer >= 400)
    {
      blinkPlayTimer = millis();
      showPlayBadge = !showPlayBadge;
    }
    if (showPlayBadge)
    {
      u8g2.drawXBMP(100, 52, play_w, play_h, play_bits);
    }
  }

  // Режим энергосберегающего скринсейвера
  void runScreenSaverFrame()
  {
    if (millis() - saverMoveTimer >= 2500)
    {
      saverMoveTimer = millis();
      saverX = random(4, 124 - nes_logo_w);
      saverY = random(4, 56 - nes_logo_h);
      saverCycles++;
      if (saverCycles >= 4)
      {
        sys.backlight(false); // Гасим подсветку для экономии батареи
      }
    }

    u8g2.clearBuffer();
    u8g2.drawXBMP(saverX, saverY, nes_logo_w, nes_logo_h, nes_logo);
    u8g2.sendBuffer();

    // Выход из скринсейвера при любом движении стика или нажатии кнопки
    if (sys.stick0Y() != 0 || sys.stick0X() != 0 || sys.sw0() || sys.sw1())
    {
      inScreenSaver = false;
      saverCycles = 0;
      sys.backlight(true);
      sys.resetIdleTimer();
      delay(200);
    }
  }

public:
  void begin(user::GameCard* gameCards, int count)
  {
    cards = gameCards;
    cardCount = count;
  }

  // Стартовая заставка со звуком на втором ядре
  void playBootScreen()
  {
    sound.play(SND_BOOT_NES);
    if (!sys.sw0() && !sys.sw1())
    {
      u8g2.clearBuffer();
      u8g2.drawXBMP(36, 16, mg_l_w, mg_l_h, mg_l);
      u8g2.sendBuffer();
      delay(1300);
    }
    gfx.clear();
  }

  // Главный цикл операционной системы (вызывается в loop())
  void tick()
  {
    if (cards == nullptr || cardCount <= 0) return;

    // 1. Если запущена игра
    if (activeGameIndex >= 0)
    {
      // Одновременное нажатие SW0 + SW1 возвращает в красивое меню
      if (sys.iSw())
      {
        sound.play(SND_MENU_MOVE);
        activeGameIndex = -1;
        sys.resetIdleTimer();
        delay(250);
        return;
      }

      cards[activeGameIndex].loopFn();
      return;
    }

    // 2. Проверка перехода в скринсейвер при бездействии в меню
    if (inScreenSaver)
    {
      runScreenSaverFrame();
      return;
    }
    if (sys.getIdleTime() >= IDLE_TIMEOUT_MS)
    {
      inScreenSaver = true;
      saverCycles = 0;
      saverMoveTimer = millis();
      return;
    }

    // 3. Навигация в главном графическом меню
    int navY = sys.stick0Y();
    int navX = sys.stick0X();
    int nav = (navY != 0) ? navY : navX;

    if (nav != 0 && (millis() - lastNavTime > 200))
    {
      lastNavTime = millis();
      if (nav > 0)
        currentIndex = (currentIndex + 1) % cardCount;
      else
        currentIndex = (currentIndex - 1 + cardCount) % cardCount;

      sound.play(SND_MENU_MOVE);
    }

    // Кнопка SW0 включает/выключает звук (Mute)
    if (sys.sw0Pressed())
    {
      sound.setMuted(!sound.isMuted());
      if (!sound.isMuted()) sound.play(SND_MENU_MOVE);
    }

    // Кнопка SW1 (или нажатие на стик) запускает выбранную игру
    if (sys.sw1Pressed())
    {
      activeGameIndex = currentIndex;
      sys.resetCoords();
      showLaunchSplash(cards[activeGameIndex].titleLine1, cards[activeGameIndex].titleLine2);
      if (cards[activeGameIndex].initFn != nullptr)
      {
        cards[activeGameIndex].initFn();
      }
      return;
    }

    // Отрисовка кадра меню
    u8g2.clearBuffer();
    drawVisualMenu();
    u8g2.sendBuffer();
  }
};

inline GraphicalOS os;
