/* Small on-device tile-map editor. Input is supplied by the host game. */
#pragma once
#include <Arduino.h>
#include "sys_gfx.h"
#include "sys_tilemap.h"

class TileMapEditor {
private:
  uint8_t* cells;
  uint8_t mapW, mapH, tileW, tileH, tileCount;
  uint8_t cursorX=0, cursorY=0, selectedTile=1;
  uint8_t viewColumns, viewRows;
public:
  TileMapEditor(uint8_t* mapData, uint8_t width, uint8_t height,
                uint8_t tileWidth, uint8_t tileHeight, uint8_t paletteCount)
    : cells(mapData), mapW(width), mapH(height), tileW(tileWidth), tileH(tileHeight), tileCount(paletteCount) {
    viewColumns = tileW ? (uint8_t)(MG_SCREEN_W / tileW) : 0;
    viewRows = tileH ? (uint8_t)(MG_SCREEN_H / tileH) : 0;
    if (tileCount) selectedTile = min((uint8_t)1, (uint8_t)(tileCount-1));
  }
  bool valid() const { return cells && mapW && mapH && tileW && tileH && tileCount; }
  void moveCursor(int8_t dx, int8_t dy) {
    if (!valid()) return;
    int16_t nx=(int16_t)cursorX+dx, ny=(int16_t)cursorY+dy;
    cursorX=(uint8_t)constrain(nx,0,(int16_t)mapW-1);
    cursorY=(uint8_t)constrain(ny,0,(int16_t)mapH-1);
  }
  void selectTile(int8_t delta) {
    if (!tileCount) return;
    int16_t next=(int16_t)selectedTile+delta;
    while (next<0) next+=tileCount;
    selectedTile=(uint8_t)(next%tileCount);
  }
  bool setSelectedTile(uint8_t id) { if (id>=tileCount) return false; selectedTile=id; return true; }
  bool paint() { if (!valid()) return false; cells[(uint16_t)cursorY*mapW+cursorX]=selectedTile; return true; }
  bool erase(uint8_t emptyTile=0) { if (!valid() || emptyTile>=tileCount) return false; cells[(uint16_t)cursorY*mapW+cursorX]=emptyTile; return true; }
  uint8_t cursorTileX() const { return cursorX; }
  uint8_t cursorTileY() const { return cursorY; }
  uint8_t currentTile() const { return selectedTile; }
  uint8_t tileAt(uint8_t x,uint8_t y) const { return (valid() && x<mapW && y<mapH) ? cells[(uint16_t)y*mapW+x] : 0; }
  // Draw a viewport from the map's upper-left corner; tile preview callback is optional.
  void draw(void (*drawTile)(uint8_t id,int16_t x,int16_t y,uint8_t w,uint8_t h)) const {
    if (!valid() || !drawTile) return;
    uint8_t cols=min(viewColumns,mapW), rows=min(viewRows,mapH);
    for (uint8_t y=0;y<rows;++y) for (uint8_t x=0;x<cols;++x)
      drawTile(cells[(uint16_t)y*mapW+x],(int16_t)x*tileW,(int16_t)y*tileH,tileW,tileH);
    if (cursorX<cols && cursorY<rows) u8g2.drawFrame((int16_t)cursorX*tileW,(int16_t)cursorY*tileH,tileW,tileH);
  }
};
