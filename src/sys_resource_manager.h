/* Fixed-capacity resource registry. It references, but never owns, resource data. */
#pragma once
#include <Arduino.h>

class GameResourceManager {
public:
  static const uint8_t MAX_RESOURCES = 32;
  enum ResourceType : uint8_t { RESOURCE_NONE = 0, RESOURCE_BITMAP, RESOURCE_TILEMAP, RESOURCE_AUDIO, RESOURCE_DATA };
  struct Resource {
    uint16_t id;
    ResourceType type;
    const void* data;
    uint16_t size;
  };
private:
  Resource entries[MAX_RESOURCES];
  uint8_t count = 0;
public:
  GameResourceManager() { for (uint8_t i=0;i<MAX_RESOURCES;++i) entries[i]={0,RESOURCE_NONE,nullptr,0}; }
  bool add(uint16_t id, ResourceType type, const void* data, uint16_t size = 0) {
    if (id == 0 || type == RESOURCE_NONE || !data) return false;
    for (uint8_t i=0;i<count;++i) if (entries[i].id == id) return false;
    if (count >= MAX_RESOURCES) return false;
    entries[count++] = {id,type,data,size}; return true;
  }
  bool replace(uint16_t id, ResourceType type, const void* data, uint16_t size = 0) {
    if (!data || type == RESOURCE_NONE) return false;
    for (uint8_t i=0;i<count;++i) if (entries[i].id == id) { entries[i]={id,type,data,size}; return true; }
    return add(id,type,data,size);
  }
  const Resource* find(uint16_t id, ResourceType expected = RESOURCE_NONE) const {
    for (uint8_t i=0;i<count;++i)
      if (entries[i].id == id && (expected == RESOURCE_NONE || entries[i].type == expected)) return &entries[i];
    return nullptr;
  }
  const void* get(uint16_t id, ResourceType expected = RESOURCE_NONE) const {
    const Resource* r=find(id,expected); return r ? r->data : nullptr;
  }
  bool remove(uint16_t id) {
    for (uint8_t i=0;i<count;++i) if (entries[i].id==id) {
      for (uint8_t j=i+1;j<count;++j) entries[j-1]=entries[j];
      --count; entries[count]={0,RESOURCE_NONE,nullptr,0}; return true;
    }
    return false;
  }
  void clear() { count=0; for (uint8_t i=0;i<MAX_RESOURCES;++i) entries[i]={0,RESOURCE_NONE,nullptr,0}; }
  uint8_t size() const { return count; }
};
