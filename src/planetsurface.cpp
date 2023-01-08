#include "planetsurface.h"

#include <cstdint>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planet.h"
#include "sprites.h"
#include "planethud.h"
#include "planetdata.h"
#include "tile.h"

int32_t PlanetSurface::getHeight(int32_t x, int32_t y) {
    return tiles[y * radius + x].z;
}

olc::Pixel PlanetSurface::getTint(int32_t x, int32_t y) {
    return tiles[y * radius + x].tint;
}

void PlanetSurface::drawTile(Tile t, olc::PixelGameEngine * e, CamParams trx) {
    t.draw(e, trx);
}

void PlanetSurface::draw(olc::PixelGameEngine * e, CamParams &trx) {
    for (int i = 0; i < radius * 2; i++) {
        for (int j = 0; j < radius * 2; j++) {
            int ia = i - radius;
            int ja = j - radius;
            if ((ia * ia + ja * ja) >= (radius * radius)) continue;
            tiles[i * radius * 2 + j].draw(e, trx);
        }
    }
    this->hud->draw(e, trx, data, selectedTile);
    this->data->draw(e, trx);
}

PlanetSurface::PlanetSurface() {
}

bool isTileDirectional(const TileType &type) {
    switch (type) {
        case TILE_ROAD: return true;
        case TILE_CABLE: return true;
        case TILE_PIPE: return true;
        default: return false;
    }
}

void PlanetSurface::updateDirectionalTiles() {
    uint32_t width = radius* 2;
    for (int i = 0; i < radius* 2; i++) {
        for (int j = 0; j < radius* 2; j++) {
            Tile &ctile = tiles[i * width + j];
            if (isTileDirectional(ctile.type)) {
                if (getType(i + 1, j) == ctile.type) ctile.state = 0;
                if (getType(i - 1, j) == ctile.type) ctile.state = 0;
                if (getType(i, j + 1) == ctile.type) ctile.state = 1;
                if (getType(i, j - 1) == ctile.type) ctile.state = 1;
                
                if (getType(i + 1, j) == ctile.type && getType(i, j + 1) == ctile.type) ctile.state = 2;
                if (getType(i + 1, j) == ctile.type && getType(i, j - 1) == ctile.type) ctile.state = 3;
                if (getType(i - 1, j) == ctile.type && getType(i, j + 1) == ctile.type) ctile.state = 4;
                if (getType(i - 1, j) == ctile.type && getType(i, j - 1) == ctile.type) ctile.state = 5;

                if (getType(i + 1, j) == ctile.type && getType(i, j + 1) == ctile.type && getType(i, j - 1) == ctile.type) ctile.state = 6;
                if (getType(i - 1, j) == ctile.type && getType(i, j + 1) == ctile.type && getType(i, j - 1) == ctile.type) ctile.state = 7;
                if (getType(i - 1, j) == ctile.type && getType(i, j + 1) == ctile.type && getType(i + 1, j) == ctile.type) ctile.state = 8;
                if (getType(i - 1, j) == ctile.type && getType(i, j - 1) == ctile.type && getType(i + 1, j) == ctile.type) ctile.state = 9;

                if (getType(i + 1, j) == ctile.type && getType(i, j + 1) == ctile.type && getType(i - 1, j) == ctile.type && getType(i, j - 1) == ctile.type) ctile.state = 10;
            } else {
                ctile.state = 0;
            }
        }
    }
}

//TODO fix the other function so this one doesnt commit the ABSOLUTE ATTROSITY of taking y before x
TileType PlanetSurface::getType(int32_t y, int32_t x) {
    if (x >= radius* 2 || y >= radius* 2 || x < 0 || y < 0) return TILE_AIR;
    return tiles[y * radius* 2 + x].type;
}

PlanetSurface::PlanetSurface(Json::Value root, uint32_t rad) {
    // std::cout << root["tiles"][0] << "\n";
    // std::cout << "parent" << p << "\n";]
    radius = rad;
    tiles.reserve(radius * radius * 4);

    for (int i = 0; i < radius* 2; i++) {
        for (int j = 0; j < radius * 2; j++) {
            Json::Value tile = root["tiles"][j + i * radius * 2];
            
            int32_t type = tile["ty"].asUInt();
            int32_t z    = tile["z"].asInt();
            uint32_t c   = tile["c"].asUInt();
            olc::Pixel colour = olc::Pixel((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
            if (rand() % 100000 == 0) {
                type = (int)TILE_TONK;
            }
            tiles.push_back(Tile((TileType)(type), z, i, j, colour));
        }
    }

    updateDirectionalTiles();

    if (!root["tileErrors"].isNull()) {
        for (uint32_t i = 0; i < root["tileErrors"].size(); i++) {
            tiles[root["tileErrors"][i]["pos"].asUInt()].addError(root["tileErrors"][i]["msg"].asString());
        }
    }
    
    this->data = new PlanetData(root);
    this->hud = new PlanetHUD();
}

void PlanetSurface::mouseOver(int max, int may, bool mouseClicked, bool mousePressed, bool rightClicked, CamParams &trx) {
    if (mouseClicked || rightClicked) {
        if (hud->mousePressed(max, may, rightClicked, trx)) {
            //HUD was clicked on, do not click on anything below HUD
            return;
        }
    }
    float mx = (max - trx.tx) / trx.zoom;
    float my = (may - trx.ty) / trx.zoom;

    tiles[lastSelectY * radius* 2 + lastSelectX].hovered = false;
    for (int i = radius* 2 - 1; i >= 0; i--) {
        for (int j = radius* 2 - 1; j >= 0; j--) {
            if (((i - radius) * (i - radius) + (j - radius) * (j - radius)) >= (this->radius * this->radius)) {
                continue;
            }
            int x = j * TEXTURE_W / 2;
            int y = i * TEXTURE_H;
            int z = tiles[i * radius* 2 + j].z;

            float scx = (x - y);
            float scy = ((x + y - TEXTURE_Z * z) / 2);

            double cx = abs(mx - (scx + TEXTURE_W / 2));
            double cy = abs(my - (scy + TEXTURE_H / 2 + TEXTURE_W / 2));
            double d  = cx / TEXTURE_W + cy / TEXTURE_H;
            if (d <= 0.5) {
                tiles[i * radius* 2 + j].hovered = true;
                lastSelectX = j;
                lastSelectY = i;
                if (hud->selectedAction != TASK_NONE && mouseClicked) {
                    data->dispatchTask(hud->selectedAction, &tiles[i * radius* 2 + j]);
                    return;
                }
                if (mouseClicked) {
                    this->hud->showClickMenu(&tiles[i * radius* 2 + j], data);
                    selectedTile = &tiles[i * radius* 2 + j];
                }
                return;
            }
        }
    }

    //if not tile and not hud
    if (mouseClicked) {
        this->hud->closeClickMenu();
        selectedTile = nullptr;
    }
}
