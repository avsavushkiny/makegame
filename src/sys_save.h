/*
  Портируемая система сохранений.
  Носитель задаётся приложением через SaveStorage. Это намеренно не привязано
  к конкретной реализации EEPROM/Flash: RP2040 core и платы используют разные API.
*/
#pragma once
#include <Arduino.h>

class SaveStorage {
public:
  virtual ~SaveStorage(){}
  virtual bool read(uint32_t address, uint8_t* data, size_t length)=0;
  virtual bool write(uint32_t address, const uint8_t* data, size_t length)=0;
};

class SaveSystem {
  struct Header { uint32_t magic; uint16_t version; uint16_t length; uint32_t checksum; };
  static const uint32_t MAGIC=0x4D475356UL; // "MGSV"
  SaveStorage* storage=nullptr;
  uint32_t baseAddress=0;
  uint32_t checksum(const uint8_t* data,size_t len)const{uint32_t h=2166136261UL;for(size_t i=0;i<len;i++){h^=data[i];h*=16777619UL;}return h;}
public:
  void attach(SaveStorage& backend,uint32_t address=0){storage=&backend;baseAddress=address;}
  template<typename T> bool save(const T& value,uint16_t version=1){
    if(!storage||sizeof(T)>65535)return false;
    Header h={MAGIC,version,(uint16_t)sizeof(T),checksum((const uint8_t*)&value,sizeof(T))};
    return storage->write(baseAddress,(const uint8_t*)&h,sizeof(h)) && storage->write(baseAddress+sizeof(h),(const uint8_t*)&value,sizeof(T));
  }
  template<typename T> bool load(T& value,uint16_t expectedVersion=1){
    if(!storage||sizeof(T)>65535)return false;
    Header h; if(!storage->read(baseAddress,(uint8_t*)&h,sizeof(h)))return false;
    if(h.magic!=MAGIC||h.version!=expectedVersion||h.length!=sizeof(T))return false;
    T temp; if(!storage->read(baseAddress+sizeof(h),(uint8_t*)&temp,sizeof(T)))return false;
    if(checksum((const uint8_t*)&temp,sizeof(T))!=h.checksum)return false;
    value=temp; return true;
  }
};

inline SaveSystem gameSave;
