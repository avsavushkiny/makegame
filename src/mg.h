/*
  makegame OS v3.0 — Объединенная платформа для консоли RP2040 (128x60 / 128x64)
  Возможности:
  - Второе ядро RP2040 (Core 1): фоновый неблокирующий синтезатор мелодий и звуковых эффектов
  - Графическое меню (GameLauncher) с иконками 48x48, котом-помощником и анимацией
  - Классы UI-элементов (Label, ProgressBar, ButtonUI, MenuList, DialogBox, IconCard)
  - Игровые классы ООП (GameObject, SpriteObject, PlayerActor, Projectile, FallingHazard)
  - Энергосберегающий скринсейвер при бездействии
*/

#pragma once

#include <Arduino.h>
#include "sys.h"
#include "sys_sound.h"
#include "sys_gfx.h"
#include "sys_xbmp.h"
#include "sys_map_xbmp.h"
#include "sys_image48.h"
#include "sys_ui.h"
#include "sys_game_obj.h"
#include "sys_def_game.h"
#include "sys_launcher.h"
