/* Менеджер уровней: фиксированный список и callback-загрузчики без динамической памяти. */
#pragma once
#include <Arduino.h>

class LevelManager {
public:
  static const uint8_t MAX_LEVELS=16;
  typedef bool (*LoadCallback)(uint8_t levelIndex);
  typedef void (*UnloadCallback)(uint8_t levelIndex);
private:
  struct Level { LoadCallback load; UnloadCallback unload; } levels[MAX_LEVELS];
  uint8_t count=0; int8_t current=-1; int8_t pending=-1;
public:
  LevelManager(){for(uint8_t i=0;i<MAX_LEVELS;i++){levels[i].load=nullptr;levels[i].unload=nullptr;}}
  int8_t addLevel(LoadCallback loadFn,UnloadCallback unloadFn=nullptr){if(count>=MAX_LEVELS||!loadFn)return -1;levels[count]={loadFn,unloadFn};return (int8_t)count++;}
  bool load(uint8_t id){if(id>=count)return false;pending=(int8_t)id;return applyPending();}
  bool next(){return current>=0 && current+1<count ? load(current+1):false;}
  bool previous(){return current>0 ? load(current-1):false;}
  bool applyPending(){
    if(pending<0)return false;
    int8_t nextId=pending;
    pending=-1;
    if(nextId<0 || nextId>=count)return false;
    if(nextId==current)return true;
    // Сначала загружаем новый уровень. Если загрузка не удалась,
    // текущий уровень остаётся активным и не теряет свои ресурсы.
    if(!levels[nextId].load((uint8_t)nextId))return false;
    int8_t oldId=current;
    current=nextId;
    if(oldId>=0 && levels[oldId].unload)levels[oldId].unload((uint8_t)oldId);
    return true;
  }
  int8_t currentLevel()const{return current;} uint8_t levelCount()const{return count;} bool hasLevel()const{return current>=0;}
};
