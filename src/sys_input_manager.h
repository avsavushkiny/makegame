/* Логические действия поверх имеющихся стиков и двух кнопок консоли. */
#pragma once
#include <Arduino.h>
#include "sys.h"

class GameInputManager {
public:
  enum Action:uint8_t { ACTION_CONFIRM=0, ACTION_CANCEL, ACTION_FIRE, ACTION_MENU, ACTION_COUNT };
  enum Button:uint8_t { BUTTON_SW0=0, BUTTON_SW1, BUTTON_NONE=255 };
private:
  Button mapping[ACTION_COUNT]={BUTTON_SW0,BUTTON_SW1,BUTTON_SW0,BUTTON_SW1};
  bool pressed[ACTION_COUNT]={false,false,false,false};
  bool previous[ACTION_COUNT]={false,false,false,false};
public:
  void bind(Action action,Button button){if(action<ACTION_COUNT)mapping[action]=button;}
  Button binding(Action action)const{return action<ACTION_COUNT?mapping[action]:BUTTON_NONE;}
  void update(){
    for(uint8_t i=0;i<ACTION_COUNT;i++){previous[i]=pressed[i];
      // Состояние кнопки читается один раз на действие; justPressed() формирует фронт.
      // Для надёжной работы вызывайте update() регулярно из главного loop().
      pressed[i]=(mapping[i]==BUTTON_SW0)?sys.sw0():(mapping[i]==BUTTON_SW1)?sys.sw1():false;
    }
  }
  bool isPressed(Action a)const{return a<ACTION_COUNT&&pressed[a];}
  bool justPressed(Action a)const{return a<ACTION_COUNT&&pressed[a]&&!previous[a];}
  int8_t axisX(uint8_t stick=0){return stick==0?sys.stick0X():sys.stick1X();}
  int8_t axisY(uint8_t stick=0){return stick==0?sys.stick0Y():sys.stick1Y();}
};

inline GameInputManager gameInput;
