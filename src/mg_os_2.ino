/*
  makegame OS v3.0 — Объединенная учебная прошивка для консоли RP2040 (128x60)
  - Core 0 (setup / loop): графическое меню с иконками 48x48, игры на классах ООП и UI-виджеты
  - Core 1 (setup1 / loop1): фоновый генератор мелодий и звуковых эффектов на пине 9

  Управление в меню:
  - Левый стик (вверх/вниз/влево/вправо): выбор игры
  - Кнопка SW1: запуск выбранной игры
  - Кнопка SW0: вкл/выкл звука (SND / MUTE)
  - Зажать SW0 + SW1 одновременно в любой игре: выход обратно в меню
*/

#include "mg.h"

/* =========================================================================
   1. ИГРА: Space Shooter (ООП: PlayerActor, Projectile, FallingHazard, UI)
   ========================================================================= */
const int MAX_METEORS = 7;
const int MAX_BULLETS = 5;

PlayerActor ship(10, 26, 8, 8, 1.8f, 3, false);
FallingHazard meteors[MAX_METEORS];
Projectile bullets[MAX_BULLETS];
ProgressBar hpBar(2, 1, 36, 6, 0, 3, false);
DialogBox gameOverBox("GAME OVER", "Press SW0 to retry", "RETRY");
Timer spaceTimer(25);
int spaceScore = 0;

void initSpaceGame()
{
  ship.reset(10, 26);
  spaceScore = 0;
  gameOverBox.setVisible(false);
  for (int i = 0; i < MAX_METEORS; i++) meteors[i].respawn();
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].setActive(false);
}

void loopSpaceGame()
{
  if (gameOverBox.isVisible())
  {
    if (sys.sw0Pressed() || sys.sw1Pressed()) initSpaceGame();
  }
  else if (spaceTimer.isReady())
  {
    ship.update();

    // Выстрел: кнопка SW0 или отклонение правого стика
    if (sys.sw0Pressed() || sys.stick1X() > 0)
    {
      for (int i = 0; i < MAX_BULLETS; i++)
      {
        if (!bullets[i].isActive())
        {
          bullets[i].fire(ship.getX() + ship.getW(), ship.getY() + 3, 3.5f, 0);
          break;
        }
      }
    }

    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].update();

    for (int i = 0; i < MAX_METEORS; i++)
    {
      meteors[i].update();

      for (int b = 0; b < MAX_BULLETS; b++)
      {
        if (bullets[b].collidesWith(meteors[i]))
        {
          bullets[b].setActive(false);
          meteors[i].respawn();
          spaceScore += 10;
          sound.play(SND_COIN);
        }
      }

      if (ship.collidesWith(meteors[i]))
      {
        meteors[i].respawn();
        ship.takeDamage(1);
        if (ship.getHp() <= 0)
        {
          sound.play(SND_GAME_OVER);
          gameOverBox.setContent("GAME OVER", "Score: " + String(spaceScore), "RETRY");
          gameOverBox.setVisible(true);
        }
      }
    }
  }

  u8g2.clearBuffer();
  hpBar.setValue(ship.getHp());
  hpBar.draw();

  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.setCursor(92, 7);
  u8g2.print("SC:");
  u8g2.print(spaceScore);

  ship.draw();
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].draw();
  for (int i = 0; i < MAX_METEORS; i++) meteors[i].draw();
  gameOverBox.draw();
  u8g2.sendBuffer();
}

/* =========================================================================
   2. ИГРА: Pong на два стика (из mg_os + звук Core 1)
   ========================================================================= */
void loopPong()
{
  u8g2.clearBuffer();
  defGame::drawField();
  defGame::drawRacket();
  defGame::calculateMovementRackets();
  defGame::drawBall();
  defGame::calculateMovementBall();
  defGame::drawScore();
  u8g2.sendBuffer();
}

/* =========================================================================
   3. ДЕМО ИНТЕРФЕЙСА: UI-виджеты (Label, ProgressBar, ButtonUI, DialogBox)
   ========================================================================= */
Label uiTitle(2, 9, "UI & SOUND DEMO", true);
ProgressBar humBar(2, 14, 96, 10, 0, 100, true);
ButtonUI btnSound(2, 28, 58, 13, "MELODY");
ButtonUI btnModal(66, 28, 58, 13, "POPUP");
DialogBox infoDialog("DUAL CORE", "Audio runs on Core1!", "OK [SW0]");
Timer demoTimer(50);
int demoValue = 25;
int demoDir = 1;

void initUiDemo()
{
  infoDialog.setVisible(false);
  btnSound.setFocus(true);
  btnModal.setFocus(false);
}

void loopUiDemo()
{
  int sx = sys.stick0X();
  if (sx < 0) { btnSound.setFocus(true);  btnModal.setFocus(false); }
  if (sx > 0) { btnSound.setFocus(false); btnModal.setFocus(true);  }

  if (infoDialog.isVisible())
  {
    if (sys.sw0Pressed() || sys.sw1Pressed()) infoDialog.setVisible(false);
  }
  else if (sys.sw0Pressed() || sys.sw1Pressed())
  {
    if (btnModal.isFocused())
    {
      sound.play(SND_MENU_SELECT);
      infoDialog.setVisible(true);
    }
    else
    {
      sound.play(SND_BOOT_NES); // Запуск мелодии в фоне на втором ядре!
      demoValue = random(15, 95);
    }
  }

  if (demoTimer.isReady())
  {
    demoValue += demoDir;
    if (demoValue >= 100 || demoValue <= 0) demoDir *= -1;
    humBar.setValue(demoValue);
  }

  u8g2.clearBuffer();
  uiTitle.draw();
  humBar.draw();
  btnSound.draw();
  btnModal.draw();
  gfx.print("L-Stick + SW0/SW1", 2, 54, 10, 6);
  infoDialog.draw();
  u8g2.sendBuffer();
}

/* =========================================================================
   4. ПРИВЕТСТВИЕ КОТА (CatDialogWidget из system_os)
   ========================================================================= */
CatDialogWidget catWidget("Hello!", "I am RP2040 Console", "Dual-Core + OOP!", "SW0: Next | SW0+1:Exit");
int catSlide = 0;

void initCatPage()
{
  catSlide = 0;
  catWidget.setLines("Hello!", "I am RP2040 Console", "Dual-Core + OOP!", "SW0: Next | SW0+1:Exit");
}

void loopCatPage()
{
  if (sys.sw0Pressed())
  {
    sound.play(SND_MENU_MOVE);
    catSlide = (catSlide + 1) % 3;
    if (catSlide == 0)
      catWidget.setLines("Hello!", "I am RP2040 Console", "Dual-Core + OOP!", "SW0: Next | SW0+1:Exit");
    else if (catSlide == 1)
      catWidget.setLines("Core 0: Graphics", "Core 1: Music Synth", "Screen: 128x60 px", "SW0: Next | SW0+1:Exit");
    else
      catWidget.setLines("Ready to create", "your own C++ game?", "Let's code!", "SW0: Next | SW0+1:Exit");
  }

  u8g2.clearBuffer();
  catWidget.draw();
  u8g2.sendBuffer();
}

/* =========================================================================
   5. ТЕСТ СТИКОВ И АЦП
   ========================================================================= */
void loopTestAdc()
{
  u8g2.clearBuffer();
  gfx.print("ADC & Buttons Test", 0, 9, 10, 6);
  u8g2.drawFrame(0, 12, 128, 48);

  if (sys.sw0())
    u8g2.drawBox(sys.joi0x(118), constrain(sys.joi0y(50), 12, 50), 10, 10);
  else
    u8g2.drawFrame(sys.joi0x(118), constrain(sys.joi0y(50), 12, 50), 10, 10);

  if (sys.sw1())
    u8g2.drawBox(sys.joi1x(112), constrain(sys.joi1y(44), 12, 44), 16, 16);
  else
    u8g2.drawFrame(sys.joi1x(112), constrain(sys.joi1y(44), 12, 44), 16, 16);

  u8g2.sendBuffer();
}

/* =========================================================================
   КАТАЛОГ КАРТОЧЕК ИГР ДЛЯ ГЛАВНОГО МЕНЮ (с иконками 48x48 из sys_image48.h)
   ========================================================================= */
user::GameCard gameCatalog[] = {
  {101, "Space",   "Shooter",  ai_bits,      loopSpaceGame, initSpaceGame},
  {102, "Pong",    "2 Sticks", pong_bits,    loopPong,      defGame::resetGame},
  {103, "UI &",    "Widgets",  dice_bits,    loopUiDemo,    initUiDemo},
  {104, "Console", "About",    jdice_bits,   loopCatPage,   initCatPage},
  {201, "Test",    "ADC/SW",   testAdc_bits, loopTestAdc,   nullptr}
};

const int GAME_COUNT = sizeof(gameCatalog) / sizeof(gameCatalog[0]);

/* =========================================================================
   ЯДРО 0 (Core 0): Инициализация железа, экрана и графического меню
   ========================================================================= */
void setup()
{
  Serial.begin(115200);
  sys.begin();
  gfx.screen();
  os.begin(gameCatalog, GAME_COUNT);
  os.playBootScreen();
}

void loop()
{
  os.tick();
}

/* =========================================================================
   ЯДРО 1 (Core 1): Выделенный неблокирующий генератор мелодий и звуков
   ========================================================================= */
void setup1()
{
  sound.beginCore1();
}

void loop1()
{
  sound.updateCore1();
}
