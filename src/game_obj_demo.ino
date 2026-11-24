/*
  GameObject Demo — минимальный пример использования
  Показывает: GameObject, SpriteObject, PlayerActor, Projectile, FallingHazard
*/

#include "mg.h"

// --- Игровые объекты ---
PlayerActor player(50, 25, 10, 10, 1.5f, 5, false); // HP=5, левый стик
Projectile bullet;
FallingHazard hazard;

// --- Таймеры ---
Timer shootTimer(200);   // интервал стрельбы
Timer spawnTimer(1000);  // интервал появления опасностей

void initDemo()
{
  player.reset(50, 25);
  bullet.setActive(false);
  hazard.respawn();
}

void loopDemo()
{
  // 1. Обновление игрока (управление левым стиком)
  player.update();

  // 2. Стрельба по кнопке SW1
  if (shootTimer.isReady() && sys.sw1Pressed())
  {
    shootTimer.reset();
    if (!bullet.isActive())
    {
      bullet.fire(
        player.getX() + player.getW(),  // старт справа от игрока
        player.getY() + player.getH() / 2,
        4.0f, 0  // скорость вправо
      );
    }
  }

  // 3. Обновление снаряда
  bullet.update();

  // 4. Обновление опасности
  hazard.update();

  // 5. Проверка коллизий
  if (bullet.isActive() && hazard.isActive() && bullet.collidesWith(hazard))
  {
    bullet.setActive(false);
    hazard.respawn();
    // Здесь можно добавить очки
  }

  if (player.collidesWith(hazard))
  {
    hazard.respawn();
    player.takeDamage(1);
  }

  // 6. Отрисовка
  u8g2.clearBuffer();

  // Игрок
  player.draw();

  // Снаряд
  bullet.draw();

  // Опасность
  hazard.draw();

  // HUD — HP
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.setCursor(2, 7);
  u8g2.print("HP: ");
  u8g2.print(player.getHp());
  u8g2.print("/");
  u8g2.print(player.getMaxHp());

  // Подсказка
  u8g2.drawFrame(0, 50, 128, 10);
  u8g2.setCursor(2, 58);
  u8g2.print("SW1: shoot | SW0+1: exit");

  u8g2.sendBuffer();

  // Выход в меню: SW0 + SW1
  if (sys.sw0Pressed() && sys.sw1Pressed())
    os.showMenu();
}
