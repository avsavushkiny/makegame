/* TileMap Engine: tile-based levels for RP2040 / U8g2. */
#pragma once
#include <Arduino.h>
#include "sys.h"
#include "sys_gfx.h"
#include "sys_engine.h"

class TileMap
{
public:
  struct Tile { const uint8_t* bitmap; bool solid; };
private:
  const uint8_t* cells;
  const Tile* tiles;
  uint8_t mapW, mapH, tileW, tileH, tileCount;
public:
  TileMap(const uint8_t* mapData, uint8_t width, uint8_t height,
          const Tile* tileSet, uint8_t count, uint8_t tileWidth = 8, uint8_t tileHeight = 8)
    : cells(mapData), tiles(tileSet), mapW(width), mapH(height), tileW(tileWidth), tileH(tileHeight), tileCount(count) {}

  uint16_t pixelWidth() const { return (uint16_t)mapW * tileW; }
  uint16_t pixelHeight() const { return (uint16_t)mapH * tileH; }
  uint8_t width() const { return mapW; }
  uint8_t height() const { return mapH; }
  uint8_t tileWidth() const { return tileW; }
  uint8_t tileHeight() const { return tileH; }

  uint8_t getTile(int tx, int ty) const {
    if (!cells || tx < 0 || ty < 0 || tx >= mapW || ty >= mapH) return 0;
    return cells[(uint16_t)ty * mapW + tx];
  }
  bool isSolid(int tx, int ty) const {
    if (tx < 0 || ty < 0 || tx >= mapW || ty >= mapH) return true;
    uint8_t id = getTile(tx, ty);
    return id < tileCount && tiles && tiles[id].solid;
  }
  bool overlapsSolid(float x, float y, int w, int h) const {
    if (w <= 0 || h <= 0) return false;
    int left = (int)floor(x / tileW), right = (int)floor((x + w - 1) / tileW);
    int top = (int)floor(y / tileH), bottom = (int)floor((y + h - 1) / tileH);
    for (int ty = top; ty <= bottom; ++ty)
      for (int tx = left; tx <= right; ++tx)
        if (isSolid(tx, ty)) return true;
    return false;
  }
  void draw(const Camera2D& camera) const {
    if (!cells || !tiles || !tileW || !tileH) return;
    int startX = max(0, (int)camera.worldX(0) / tileW);
    int startY = max(0, (int)camera.worldY(0) / tileH);
    int endX = min((int)mapW - 1, (int)camera.worldX(MG_SCREEN_W - 1) / tileW);
    int endY = min((int)mapH - 1, (int)camera.worldY(MG_SCREEN_H - 1) / tileH);
    for (int ty = startY; ty <= endY; ++ty) for (int tx = startX; tx <= endX; ++tx) {
      uint8_t id = getTile(tx, ty);
      if (id < tileCount && tiles[id].bitmap)
        u8g2.drawXBMP(camera.screenX(tx * tileW), camera.screenY(ty * tileH), tileW, tileH, tiles[id].bitmap);
    }
  }
};
