# makegame OS — расширение игрового движка

В проект добавлен `sys_engine.h`. Подсистемы рассчитаны на RP2040, Arduino и дисплей 128×60. Они не используют `String`, `std::vector`, `new` или динамическое выделение памяти.

## Что добавлено

1. **EngineClock** — обновление игровой логики с фиксированным шагом. Это делает скорость физики менее зависимой от частоты отрисовки.
2. **Camera2D** — камера для уровней, которые больше экрана; поддерживает слежение за объектом и отсечение невидимых объектов.
3. **SpriteAnimation** — покадровая анимация из массива XBM-спрайтов одинакового размера.
4. **ParticleSystem** — эффекты взрыва, искр и попаданий на заранее выделенном массиве частиц.
5. **GameStateManager** — состояния меню, игры, паузы и завершения игры с отдельными функциями входа, обновления и отрисовки.
6. **Дополнительные методы GameObject** — скорость по осям, изменение размера, проверка попадания точки и ограничение координат экраном.
7. **Функции геометрии** — проверка точки в прямоугольнике и пересечение двух окружностей.

## Подключение

`sys_engine.h` уже подключён из `mg.h`. В игровом `.ino` достаточно `#include "mg.h"`.

## Пример фиксированного обновления

```cpp
EngineClock gameClock(25); // 25 мс, около 40 обновлений логики в секунду

void updateWorld()
{
  ship.update();
  for (int i = 0; i < MAX_METEORS; i++) meteors[i].update();
}

void loopMyGame()
{
  gameClock.update(updateWorld);
  u8g2.clearBuffer();
  ship.draw();
  for (int i = 0; i < MAX_METEORS; i++) meteors[i].draw();
  u8g2.sendBuffer();
}
```

Вызывать `gameClock.begin()` следует при запуске/сбросе игры. Не вызывайте `updateWorld()` дополнительно из другого места, иначе логика будет обновляться дважды.

## Пример камеры

```cpp
Camera2D camera;

void drawLevel()
{
  camera.follow(ship, 512, 128); // размер игрового мира
  camera.draw(ship);
  // Для спрайта: u8g2.drawXBMP(camera.screenX(x), camera.screenY(y), w, h, bitmap);
}
```

Камера хранит смещение мира, но не меняет координаты игровых объектов. Для всех объектов уровня применяйте `screenX()` и `screenY()` при рисовании.

## Пример анимации

```cpp
extern const uint8_t hero0[];
extern const uint8_t hero1[];
extern const uint8_t hero2[];
const uint8_t* heroFrames[] = { hero0, hero1, hero2 };
SpriteAnimation heroAnimation(heroFrames, 3, 140, true);

// В игровом обновлении:
heroAnimation.update();
// В отрисовке:
heroAnimation.draw(20, 20, 8, 8);
```

Все кадры должны иметь одинаковые ширину/высоту, соответствующие параметрам `draw()`.

## Пример частиц

```cpp
ParticleSystem::Particle particles[24];
ParticleSystem sparks(particles, 24);

// При попадании:
sparks.emit(meteor.getX(), meteor.getY(), 8, 20);
// Каждый кадр:
sparks.update();
// При отрисовке:
sparks.draw();
```

Пул фиксированного размера: при переполнении новые частицы заменяют старые. Увеличивайте массив только с учётом доступной RAM.

## Пример состояний игры

```cpp
GameStateManager gameStates;

void enterPlay() { /* сброс игровых параметров */ }
void updatePlay() { /* игровая логика */ }
void drawPlay() { /* отрисовка */ }

void setupGameStates()
{
  gameStates.configure(ENGINE_PLAYING, enterPlay, updatePlay, drawPlay);
  gameStates.change(ENGINE_PLAYING);
}

// В основном цикле игры:
// gameStates.update();
// u8g2.clearBuffer(); gameStates.draw(); u8g2.sendBuffer();
// gameStates.change(ENGINE_PAUSED);
```

## Рекомендации по развитию

- Для новых игр используйте `GameStateManager`, чтобы не смешивать паузу, игровой процесс и экран результата в одном наборе условий.
- Для физики и движения используйте фиксированный шаг `EngineClock`; для анимаций интерфейса можно оставить `millis()`.
- Не создавайте большие массивы объектов на стеке. Для пула пуль, врагов и частиц предпочтительны статические массивы.
- `GameObject::collidesWith()` использует прямоугольники AABB. Для круглых объектов применяйте `mgCircleHit()`.
- Звуковые вызовы остаются в существующем `SoundEngineCore1`; новая подсистема звука не добавлялась, чтобы не менять работу второго ядра.

## Важное замечание

Архив содержит исходники, но не содержит точной конфигурации платы, версии Arduino-Pico core и библиотек. Поэтому изменения проверены на уровне структуры и совместимости исходного API, но окончательная проверка компиляции и работы должна выполняться в вашей установленной среде Arduino IDE.


## Новые подсистемы: TileMap, Physics и ObjectManager

### 1. TileMap Engine

Уровень хранится как массив идентификаторов тайлов. Размеры карты и тайла задаются при создании объекта. Для каждого типа тайла указывается XBM-картинка и признак `solid`.

```cpp
const uint8_t tileEmpty[] = { /* XBM bytes for 8x8 */ };
const uint8_t tileWall[]  = { /* XBM bytes for 8x8 */ };
const TileMap::Tile tileSet[] = {
  { tileEmpty, false },
  { tileWall, true }
};
const uint8_t level[] = {
  1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,1,
  1,0,0,1,1,0,0,1,
  1,1,1,1,1,1,1,1
};
TileMap map(level, 8, 4, tileSet, 2, 8, 8);
Camera2D camera;

void drawLevel() {
  map.draw(camera);
}
```

`getTile(tx, ty)` возвращает ID тайла, `isSolid(tx, ty)` проверяет его твёрдость, `overlapsSolid(x, y, w, h)` проверяет прямоугольник в мировых координатах. Выход за границы карты считается твёрдой границей. Карта и набор тайлов должны оставаться в памяти всё время использования `TileMap`.

### 2. GamePhysics

`PhysicsBody` добавляет к `GameObject` ускорение свободного падения, горизонтальное трение, прыжок, ограничение скорости падения и простой отскок. Столкновения с картой разрешаются отдельно по X и Y, что удобно для платформеров.

```cpp
GameObject hero(16, 8, 6, 8);
PhysicsBody heroPhysics(hero, 0.18f, 0.82f);
heroPhysics.setMaxFallSpeed(3.5f);

void updateGame() {
  heroPhysics.update(map);
  if (heroPhysics.isGrounded()) {
    // персонаж стоит на поверхности
  }
  // Прыжок по событию кнопки:
  // heroPhysics.jump(2.8f);
}
```

Для движения по горизонтали задавайте скорость через `hero.setVelocityX(...)`. `jump()` срабатывает только при `isGrounded()`. Вызов `update(map)` выполняйте с постоянным шагом, например через `EngineClock`. Это облегчённая физика платформера, а не полноценный физический симулятор: нет наклонных поверхностей, вращения и импульсного разрешения столкновений между телами.

### 3. GameObjectManager

Менеджер хранит указатели на объекты в переданном массиве фиксированного размера. Он не создаёт и не удаляет объекты и не владеет их памятью — объекты можно объявлять статически, что предпочтительно для микроконтроллера.

```cpp
GameObject enemy1(40, 20, 6, 6);
GameObject enemy2(60, 20, 6, 6);
GameObject* objectSlots[12];
GameObjectManager objects(objectSlots, 12);

void setupGame() {
  objects.add(hero);
  objects.add(enemy1);
  objects.add(enemy2);
}
void updateGame() {
  objects.update();
  objects.checkCollisions(onCollision);
  objects.removeInactive(); // удаляет только из списка менеджера
}
void drawGame() {
  objects.draw();
}
```

Функция обратного вызова столкновения имеет сигнатуру `void onCollision(GameObject& a, GameObject& b)`. `removeInactive()` исключает неактивные объекты из списка, но не освобождает память. После удаления объект можно снова зарегистрировать через `add()`.

### Примечания по памяти и порядку обновления

- Все три подсистемы используют заранее выделенные массивы и не вызывают `new`, `delete` или `std::vector`.
- Один объект не следует одновременно обновлять вручную и через `GameObjectManager`.
- Если применяется `PhysicsBody`, не вызывайте для этого же объекта обычный `update()` менеджера: физический контроллер сам перемещает объект.
- `TileMap::draw()` выводит только видимые тайлы в пределах камеры. XBM-данные тайлов должны соответствовать указанным ширине и высоте.
- Для отрисовки спрайтов с камерой используйте `u8g2.drawXBMP(camera.screenX(obj.getX()), camera.screenY(obj.getY()), obj.getW(), obj.getH(), bitmap)`.

## Следующий этап: SceneManager, AnimationController и GameEventSystem

В этой версии добавлены три независимые подсистемы. Все они работают без `new`/`malloc` и используют фиксированные структуры данных.

### 4. SceneManager — управление сценами

Файл: `sys_scene.h`.

Менеджер хранит до 8 сцен. Для каждой сцены задаются функции входа, выхода, обновления и отрисовки. Функции могут быть обычными статическими функциями или функциями уровня файла.

```cpp
SceneManager scenes;
int8_t menuScene, gameScene, pauseScene;

void menuEnter() { }
void menuExit() { }
void menuUpdate() { /* обработка меню */ }
void menuDraw() { /* отрисовка меню */ }

void gameEnter() { /* сброс/подготовка уровня */ }
void gameExit() { }
void gameUpdate() { /* игровая логика */ }
void gameDraw() { /* отрисовка уровня */ }

void setupScenes() {
  menuScene = scenes.addScene(menuEnter, menuExit, menuUpdate, menuDraw);
  gameScene = scenes.addScene(gameEnter, gameExit, gameUpdate, gameDraw);
  scenes.changeTo(menuScene);
}

void loop() {
  scenes.update();
  u8g2.clearBuffer();
  scenes.draw();
  u8g2.sendBuffer();
}
```

Переход выполняется через `changeTo(sceneId)`. Если запрос перехода поступил из callback текущей сцены, он будет применён после завершения текущего callback, а не посреди его выполнения. `addScene()` возвращает индекс сцены или `-1`, если достигнут лимит.

### 5. AnimationController — контроллер анимаций

Файл: `sys_animation.h`.

Контроллер переключает кадры `XBM` у объекта `SpriteObject`. Все кадры одного клипа должны иметь одинаковые размеры, соответствующие размерам объекта.

```cpp
extern const uint8_t hero_idle_0[];
extern const uint8_t hero_idle_1[];
extern const uint8_t hero_walk_0[];
extern const uint8_t hero_walk_1[];

const uint8_t* const idleFrames[] = { hero_idle_0, hero_idle_1 };
const uint8_t* const walkFrames[] = { hero_walk_0, hero_walk_1 };

const AnimationController::Clip heroClips[] = {
  { idleFrames, 2, 250, true }, // 0: ожидание
  { walkFrames, 2, 100, true }  // 1: ходьба
};

SpriteObject hero(20, 20, 8, 8, hero_idle_0);
AnimationController heroAnimation(hero, heroClips, 2);

void setup() {
  heroAnimation.play(0); // начать Idle
}

void loop() {
  heroAnimation.update();
  // В игровом цикле вызвать hero.draw().
}
```

Методы: `play(id, restart)`, `pause()`, `resume()`, `stop()`, `update()`, `isPlaying()`, `currentFrame()` и `currentAnimation()`. Для однократного клипа укажите `loop = false`: после последнего кадра воспроизведение остановится.

### 6. GameEventSystem — очередь игровых событий

Файл: `sys_events.h`.

Очередь вмещает 16 событий. Событие содержит тип, идентификатор источника, идентификатор цели и целочисленное значение. Это позволяет игровым объектам сообщать о событиях, не вызывая напрямую код меню, уровня или интерфейса.

```cpp
GameEventSystem gameEvents;

void onGameEvent(const GameEvent& event) {
  if (event.type == GAME_EVENT_ITEM_COLLECTED) {
    // event.source — кто подобрал предмет
    // event.target — идентификатор предмета
    // event.value — например, количество очков
  }
  else if (event.type == GAME_EVENT_PLAYER_DIED) {
    // здесь можно запросить переход на экран Game Over
  }
}

void setup() {
  gameEvents.setListener(onGameEvent);
}

void loop() {
  // Внутри игровой логики:
  // gameEvents.post(GAME_EVENT_ITEM_COLLECTED, 0, 4, 10);

  // Обработать накопившиеся события:
  gameEvents.dispatch();
}
```

Доступные типы: `GAME_EVENT_COLLISION`, `GAME_EVENT_DAMAGE`, `GAME_EVENT_DESTROYED`, `GAME_EVENT_ITEM_COLLECTED`, `GAME_EVENT_LEVEL_COMPLETE`, `GAME_EVENT_PLAYER_DIED` и `GAME_EVENT_CUSTOM`. `post()` возвращает `false`, если очередь заполнена или передан `GAME_EVENT_NONE`. Число потерянных событий можно получить через `droppedEvents()`. `dispatch(limit)` вызывает установленный обработчик не более `limit` раз за вызов.

### Рекомендуемый порядок игрового кадра

1. Считать управление.
2. Обновить физику и игровые объекты.
3. Обновить контроллеры анимации.
4. Опубликовать и обработать игровые события.
5. Выполнить `SceneManager.update()`.
6. Очистить буфер, вызвать `SceneManager.draw()` и отправить буфер на дисплей.

Не вызывайте `dispatch()` из обработчика события повторно: обрабатывайте события одним центральным местом игрового цикла. Если используется `EngineClock`, обновления физики и объектов лучше выполнять в его фиксированном callback, а отрисовку оставить в основном цикле.

### Ограничения

- `SceneManager` рассчитан максимум на 8 зарегистрированных сцен.
- `AnimationController` управляет одним `SpriteObject`; для нескольких объектов создавайте отдельные контроллеры.
- `GameEventSystem` имеет очередь на 16 событий и одного общего обработчика.
- Эти подсистемы добавлены в `mg.h`, поэтому отдельно подключать их заголовки при использовании общего `mg.h` не требуется.
- Перед использованием на плате необходимо проверить компиляцию с установленной версией Arduino core и библиотекой U8g2.

---

# Этап 4. Игровые сервисы

## GameAudioManager (`sys_audio_manager.h`)

Единая точка вызова игровых звуков:

```cpp
gameAudio.begin();
gameAudio.setMasterVolume(80);
gameAudio.play(SND_COIN, GameAudioManager::PRIORITY_HIGH);
gameAudio.setMuted(true);
```

Важно: текущий `SoundEngineCore1` использует один пьезоизлучатель и воспроизводит одну последовательность за раз. Поэтому это не микшер и не многоканальный PCM-плеер. Приоритеты применяются к запросам, а не к точному времени окончания текущей мелодии; master volume пока является логическим уровнем 0..100 (аппаратная громкость не регулируется).

## Игровые UI-компоненты (`sys_game_ui.h`)

```cpp
HealthBar hp(2, 2, 45, 7, 100, true);
LivesCounter lives(90, 2, 3);
ScoreCounter score(2, 55);
InventorySlots inventory(2, 38, 4);

hp.setValue(75);
lives.setLives(2);
score.add(100);
inventory.setItem(0, 1);
inventory.select(0);
// В игровом draw:
hp.draw(); lives.draw(); score.draw(); inventory.draw();
```

Компоненты рисуются в текущий framebuffer U8g2; обновление значений выполняет сама игра.

## LevelManager (`sys_level_manager.h`)

```cpp
bool loadForest(uint8_t) { /* подготовить карту и объекты */ return true; }
void unloadForest(uint8_t) { /* освободить ресурсы уровня */ }
LevelManager levels;
int8_t forestId = levels.addLevel(loadForest, unloadForest);
levels.load(forestId);
levels.next();
```

Загрузчик уровня возвращает `true` при успехе. Новый уровень загружается до выгрузки текущего: если загрузчик вернул `false`, текущий уровень остаётся активным. После успешной загрузки вызывается callback выгрузки предыдущего уровня. Поэтому загрузчик должен подготовить новый набор ресурсов так, чтобы он мог временно сосуществовать со старым. Сохранение/восстановление прогресса выполняется отдельно через SaveSystem.

## SaveSystem (`sys_save.h`)

Система сериализует POD-структуру (простую структуру без `String`, указателей и виртуальных методов), записывая заголовок с magic, версией, размером и контрольной суммой FNV-1a.

```cpp
struct PlayerSave { uint32_t score; uint8_t lives; uint8_t level; };
PlayerSave state = {1200, 3, 2};
// Сначала подключить реализацию SaveStorage для выбранного ядра RP2040:
// gameSave.attach(myStorage, адрес);
// gameSave.save(state, 1);
// gameSave.load(state, 1);
```

`SaveStorage` — абстрактный интерфейс. Конкретный адаптер EEPROM/Flash намеренно не включён: доступ к flash и резервирование области зависят от выбранного RP2040 Arduino core и схемы разметки прошивки. Не записывайте в произвольный адрес flash без выделенной области; учитывайте ресурс циклов стирания.

## GameInputManager (`sys_input_manager.h`)

```cpp
gameInput.bind(GameInputManager::ACTION_CONFIRM, GameInputManager::BUTTON_SW1);
gameInput.update();
if (gameInput.justPressed(GameInputManager::ACTION_CONFIRM)) { /* подтвердить */ }
int dx = gameInput.axisX(0); // -2..2, левый стик
int dy = gameInput.axisY(1); // -2..2, правый стик
```

Вызывайте `gameInput.update()` один раз за проход главного `loop()`. `justPressed()` сообщает фронт нажатия, `isPressed()` — текущее удержание. Если нескольким действиям назначена одна кнопка, все эти действия получают одно состояние.

## Подключение

Все пять заголовков подключены в `mg.h`. Это header-only компоненты, динамическое выделение памяти в них не используется. Перед использованием `gameSave` подключите конкретный backend `SaveStorage` под выбранную версию Arduino core RP2040.


## Этап 5 — инструменты разработки игр

Добавлены пять вспомогательных подсистем. Все они используют фиксированные массивы и ссылки на данные, которыми владеет приложение; автоматического выделения памяти и копирования больших ресурсов нет.

### 1. SpriteSheetManager (`sys_spritesheet.h`)

Регистрирует наборы кадров XBM одинакового размера и позволяет получить кадр или сразу нарисовать его. Здесь под SpriteSheet понимается подготовленный массив отдельных XBM-кадров, а не декодер PNG/BMP-файла или извлекатель прямоугольников из единого bitmap.

```cpp
const uint8_t* const heroFrames[] = { hero0, hero1, hero2 };
SpriteSheetManager spriteSheets;
int8_t heroSheet;
void setupSprites() {
  heroSheet = spriteSheets.add(heroFrames, 3, 8, 8);
}
void drawHeroFrame(uint8_t frame) {
  spriteSheets.draw(heroSheet, frame, 20, 20);
}
```

### 2. TileMapEditor (`sys_tilemap_editor.h`)

Минимальный редактор карты непосредственно на консоли: перемещение курсора, выбор ID тайла, рисование и стирание. В отличие от `TileMap`, редактору передаётся изменяемый массив `uint8_t*`. Размер массива должен быть не меньше `width * height`, а данные и массив должны жить всё время работы редактора.

```cpp
uint8_t editableLevel[16 * 8] = {0};
TileMapEditor editor(editableLevel, 16, 8, 8, 8, 4);

// Привязка к вашей системе ввода:
// editor.moveCursor(-1, 0); editor.moveCursor(1, 0);
// editor.selectTile(+1); editor.paint(); editor.erase();
void drawOneTile(uint8_t id, int16_t x, int16_t y, uint8_t w, uint8_t h) {
  // Здесь отрисуйте XBM выбранного тайла по id.
  if (id == 1) u8g2.drawFrame(x, y, w, h);
}
void drawEditor() { editor.draw(drawOneTile); }
```

Редактор показывает верхнюю левую часть карты, помещающуюся на экран; прокрутка больших карт и сохранение на Flash должны быть организованы приложением отдельно. `drawTile` — callback приложения, связывающий ID тайла с конкретным изображением.

### 3. GameResourceManager (`sys_resource_manager.h`)

Реестр до 32 ресурсов по числовому ID. Поддерживаются типы bitmap, tilemap, audio и произвольные данные. Менеджер хранит только указатель, тип и размер; он не загружает файлы, не освобождает память и не продлевает срок жизни ресурсов.

```cpp
GameResourceManager resources;
resources.add(1, GameResourceManager::RESOURCE_BITMAP, hero0, sizeof(hero0));
const uint8_t* hero = (const uint8_t*)resources.get(
  1, GameResourceManager::RESOURCE_BITMAP);
```

ID `0` зарезервирован. Повторная регистрация ID через `add()` отклоняется; для обновления используйте `replace()`.

### 4. GameDialog (`sys_dialog.h`)

Диалог с несколькими страницами текста и необязательным набором до четырёх вариантов ответа. Ввод не считывается внутри класса: приложение вызывает `nextPage()`, `previousPage()`, `moveChoice()` и `confirm()` по событиям кнопок.

```cpp
const char* introPages[] = { "Hello, player!", "Find the key." };
const char* answers[] = { "Continue", "Cancel" };
GameDialog dialog;
void onDialogChoice(uint8_t choice) { /* обработать выбор */ }
void openDialog() { dialog.open(introPages, 2, answers, 2, onDialogChoice); }
void drawGameUI() { dialog.draw(); }
```

Строки и массивы строк должны оставаться действительными, пока диалог открыт. Для кириллицы замените стандартный шрифт в `draw()` на шрифт U8g2 с кириллическими глифами, доступный в установленной версии библиотеки. Автоматическая локализация и перенос слов пока не реализованы.

### 5. GameDebugger (`sys_debugger.h`)

Отладочная панель показывает FPS, число зарегистрированных игровых объектов, число столкновений и длительность обновления в микросекундах. Счётчики столкновений и объектов передаются приложением.

```cpp
GameDebugger debugger;
void setupDebugger() { debugger.begin(); debugger.enable(true); }
void updateGame() {
  debugger.beginUpdate();
  objects.update();
  objects.checkCollisions(onCollision);
  debugger.sampleObjects(objects);
  debugger.endUpdate();
}
void drawGame() {
  // Сначала отрисуйте игру.
  debugger.frame();
  debugger.draw(); // последним, поверх игрового изображения
}
```

`setCollisionCount()` позволяет передать собственный счётчик столкновений. Для измерения всего игрового обновления вызовы `beginUpdate()` и `endUpdate()` должны охватывать именно тот участок, который требуется измерить. Автоматическое измерение свободной RAM намеренно не включено: доступные способы зависят от версии Arduino-Pico core и линкера.

### Ограничения этапа

- Редактор изменяет RAM-массив карты; запись/чтение проекта из Flash/файловой системы не добавлены.
- Реестр ресурсов не является файловым загрузчиком и не управляет временем жизни ресурсов.
- SpriteSheetManager принимает массивы отдельных XBM-кадров; упаковка/распаковка атласа не выполняется.
- Диалоги используют строки приложения и не имеют встроенного парсера сценариев.
- Панель отладчика не заменяет аппаратный профилировщик и не оценивает использование памяти.
