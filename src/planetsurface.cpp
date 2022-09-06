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
    double xb = x - this->rad;
    double yb = y - this->rad;
    double noise = noiseGen.GetNoise(xb / parent->generationNoise[0], yb / parent->generationNoise[0], (double)parent->generationZValues[0]);
    int32_t height = noise * 30;
    if (height < parent->seaLevel) height = parent->seaLevel;
    return height;
}

olc::Pixel PlanetSurface::getTint(int32_t x, int32_t y) {
    if (getHeight(x, y) <= parent->seaLevel) {
        //water, return b l u e
        return parent->generationColours[0];
    }
    int xb = x - parent->radius;
    int yb = y - parent->radius;

    int r = 0;
    int g = 0;
    int b = 0;
    int total = 0;
    for (int i = 1; i < parent->numColours; i++) {
        double noiseVal = (noiseGen.GetNoise(
            xb / parent->generationNoise[i],
            yb / parent->generationNoise[i],
            (double)parent->generationZValues[i]) + 1) / 2;

        if (noiseVal > parent->generationChances[i]) {
            r += parent->generationColours[i].r;
            g += parent->generationColours[i].g;
            b += parent->generationColours[i].b;
            total += 1;
        }
    }
    if (total == 0) {
        r = parent->baseColour.r;
        g = parent->baseColour.g;
        b = parent->baseColour.b;
    } else {
        r /= total;
        g /= total;
        b /= total;
    }

   return olc::Pixel(r, g, b);
}

void PlanetSurface::drawTile(Tile t, olc::PixelGameEngine * e, CamParams trx) {
    t.draw(e, trx);
}

void PlanetSurface::draw(olc::PixelGameEngine * e, CamParams &trx) {
    for (int i = 0; i < parent->radius * 2; i++) {
        for (int j = 0; j < parent->radius * 2; j++) {
            int ia = i - parent->radius;
            int ja = j - parent->radius;
            if ((ia * ia + ja * ja) >= (parent->radius * parent->radius)) continue;
            tiles[i * parent->radius * 2 + j].draw(e, trx);
        }
    }
    this->hud->draw(e, trx);
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
    uint32_t width = rad * 2;
    for (int i = 0; i < rad * 2; i++) {
        for (int j = 0; j < rad * 2; j++) {
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
    if (x >= rad * 2 || y >= rad * 2 || x < 0 || y < 0) return TILE_AIR;
    return tiles[y * rad * 2 + x].type;
}

PlanetSurface::PlanetSurface(Json::Value root, Planet * p) {
    parent = p;
    int width = root["rad"].asInt() * 2;
    tiles.reserve(width * width);
    rad = root["rad"].asInt();

    for (int i = 0; i < root["rad"].asInt() * 2; i++) {
        for (int j = 0; j < root["rad"].asInt() * 2; j++) {
            uint64_t val = root["tiles"][j + i * root["rad"].asInt() * 2].asUInt64();
            int32_t type = val & 0xFFFFFFFF;
            int32_t z    = (val >> 32) & 0xFFFFFFFF;
            if (rand() % 100000 == 0) {
                type = (int)TILE_TONK;
            }
            tiles.push_back(Tile((TileType)(type), z, j, i, this->getTint(i, j))); //TODO WHY DO I HAVE TO SWAP J AND I HERE?
        }
    }

    updateDirectionalTiles();

    if (!root["tileErrors"].isNull()) {
        for (uint32_t i = 0; i < root["tileErrors"].size(); i++) {
            tiles[root["tileErrors"][i]["pos"].asUInt()].addError(root["tileErrors"][i]["msg"].asString());
        }
    }
    

    this->data = new PlanetData(this, root);
    this->hud = new PlanetHUD(this, this->data);
    generated = true;
    requested = false;
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

    tiles[lastSelectY * rad * 2 + lastSelectX].hovered = false;


    //float ax = /*WHY THE CINNAMON TOAST FUCK DO I NEED THIS?!*/// WIDTH - (mx - trx.tx) / trx.zoom;
    //float ay = (my - trx.ty) / trx.zoom;
    //int wx = (ay / 64.0) + (ax / 128.0);
    //int wy = (-ax / 128.0) + (ay / 64.0);
    //The analysis is severely limited by my lack of understanding of what I am doing
    //this is utterly fucking stupid but it works. DON'T U FUCKING DARE TOUCH THIS CODE IT TOOK ME 2 HOURS
    //wx -= 8;
    //wy += 6;


    //if (wx < 0 || wy < 0 || wx > radius * 2 || wy > radius * 2) {
//      return;
    //}


    //tiles[wy * radius * 2 + wx] = 3;
    //lastSelectX = wx;
    //lastSelectY = wy;
    /*

    int MAXITER = 4;

    for (int ia = MAXITER; ia >= 0; ia--) {
        for (int ja = 2; ja >= -2; ja--) {
            int i = ia + wx;
            int j = ja + ia + wy;*/

    for (int i = rad * 2 - 1; i >= 0; i--) {
        for (int j = rad * 2 - 1; j >= 0; j--) {
            if (((i - rad) * (i - rad) + (j - rad) * (j - rad)) >= (this->rad * this->rad)) {
                continue;
            }
            int x = j * TEXTURE_W / 2;
            int y = i * TEXTURE_H;
            int z = tiles[i * rad * 2 + j].z;

            float scx = (x - y);
            float scy = ((x + y - TEXTURE_Z * z) / 2);

            double cx = abs(mx - (scx + TEXTURE_W / 2));
            double cy = abs(my - (scy + TEXTURE_H / 2 + TEXTURE_W / 2));
            double d  = cx / TEXTURE_W + cy / TEXTURE_H;
            if (d <= 0.5) {
                tiles[i * rad * 2 + j].hovered = true;
                lastSelectX = j;
                lastSelectY = i;
                if (hud->selectedAction != TASK_NONE && mouseClicked) {
                    data->dispatchTask(hud->selectedAction, &tiles[i * rad * 2 + j]);
                    return;
                }
                if (mouseClicked) {
                    this->hud->showClickMenu(&tiles[i * rad * 2 + j]);
                    selectedTile = &tiles[i * rad * 2 + j];
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
/*
    //try to calculate without using
    tiles[lastSelectY * radius * 2 + lastSelectX] = 0;
    float ax = (mx - trx.tx) / trx.zoom;
    float ay = (my - trx.ty) / trx.zoom;
    int wx = (2 * ay - ax) / 2 / 64;
    int wy = (2 * ay + ax) / 2 / 64;
    if (wx < 0 || wy < 0) {
        return;
    }
    int MAXITER = 4;

    int offset = MAXITER;
    for (int ia = MAXITER; ia >= 0; ia--) {
        for (int ja = 1; ja >= -1; ja--) {
            int i = ia + wx;
            int j = ja + offset + wy;

            int xb = i - radius;
            int yb = j - radius;
            int x = i * 64;
            int y = j * 64;
            float az = (1 - (noiseGen.GetNoise(xb / parent->generationNoise[pos], yb / parent->generationNoise[pos], parent->generationZValues[pos]) + 1) / 2) - (1 - parent->generationChances[pos]);
            int z = az * 30;
            if (z < 0) {
                z = -z;
            }

            float scx = (x - y) * trx.zoom + trx.tx;
            float scy = ((x + y - 30 * z) / 2) * trx.zoom + trx.ty;
            float deltax = mx - scx;
            float deltay = my - scy;
            //std::cout << "deltay " << deltay << " scy " << my << "\n";
            if ((deltax <= (64 * trx.zoom)) && (deltay <= (64 * trx.zoom)) && deltay >= 0 && deltax >= 0) {
                //selected tile
                tiles[i * radius * 2 + j] = 3;
                lastSelectX = j;
                lastSelectY = i;
                return;
            }
        }
        offset--;
    }
*/
