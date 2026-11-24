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
#include "sys_engine.h"
#include "sys_tilemap.h"
#include "sys_physics.h"
#include "sys_object_manager.h"

#include "sys_scene.h"
#include "sys_animation.h"
#include "sys_events.h"

// Дополнительные игровые сервисы (этап 4)
#include "sys_audio_manager.h"
#include "sys_game_ui.h"
#include "sys_level_manager.h"
#include "sys_save.h"
#include "sys_input_manager.h"

// Инструменты разработки игр (этап 5)
#include "sys_spritesheet.h"
#include "sys_tilemap_editor.h"
#include "sys_resource_manager.h"
#include "sys_dialog.h"
#include "sys_debugger.h"
