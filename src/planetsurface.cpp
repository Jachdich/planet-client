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
#include "common/planetdata.h"
#include "tile.h"

olc::Pixel PlanetSurface::getTint(int x, int y) {
    int xb = x - parent->radius;
    int yb = y - parent->radius;

    int r = 0;
    int g = 0;
    int b = 0;
    int total = 0;
    for (int i = 0; i < parent->numColours; i++) {
        if ((noiseGen.GetNoise(xb / parent->generationNoise[i], yb / parent->generationNoise[i], parent->generationZValues[i]) + 1) / 2 > parent->generationChances[i]) {
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

void PlanetSurface::draw(olc::PixelGameEngine * e, CamParams trx) {
    for (int i = 0; i < parent->radius * 2; i++) {
        for (int j = 0; j < parent->radius * 2; j++) {
            int ia = i - parent->radius;
            int ja = j - parent->radius;
            if ((ia * ia + ja * ja) >= (parent->radius * parent->radius)) continue;
            drawTile(tiles[i * parent->radius * 2 + j], e, trx);
        }
    }
	this->hud->draw(e, trx);
    this->data->draw(e, trx);
}

PlanetSurface::PlanetSurface() {
}

PlanetSurface::PlanetSurface(Json::Value root, Planet * p) {
	parent = p;
    int width = root["rad"].asInt() * 2;
	tiles.reserve(width * width);
	for (int i = 0; i < root["rad"].asInt() * 2; i++) {
		for (int j = 0; j < root["rad"].asInt() * 2; j++) {
			uint64_t val = root["tiles"][i + j * root["rad"].asInt() * 2].asUInt64();
			int32_t type = val & 0xFFFFFFFF;
            int32_t z    = (val >> 32) & 0xFFFFFFFF;
			tiles.push_back(Tile((TileType)(type), z, j, i, this->getTint(j, i)));
		}
	}
	generated = true;
	requested = false;
	rad = root["rad"].asInt();

	this->data = new PlanetData(this);
	this->hud = new PlanetHUD(this, this->data);
    //this->startThread();
}

void PlanetSurface::startThread() {
    std::thread(&PlanetData::runLogic, this->data).detach();
    this->threadStarted = true;
    this->data->threadStopped = false;
}

void PlanetSurface::cleanUpThread() {
    this->data->stopThread();
    this->threadStarted = false;
}

void PlanetSurface::mouseOver(int max, int may, bool mouseClicked, bool mousePressed, CamParams trx) {
	if (mouseClicked) {
		if (hud->mousePressed(max, may, trx)) {
			return;
		}
	}
	float mx = (max - trx.tx) / trx.zoom;
	float my = (may - trx.ty) / trx.zoom;

	tiles[lastSelectY * rad * 2 + lastSelectX].hovered = false;


	//float ax = /*WHY THE CINNAMON TOAST FUCK DO I NEED THIS?!*/ WIDTH - (mx - trx.tx) / trx.zoom;
	//float ay = (my - trx.ty) / trx.zoom;
	//int wx = (ay / 64.0) + (ax / 128.0);
	//int wy = (-ax / 128.0) + (ay / 64.0);

	//this is utterly fucking stupid but it works. DON'T U FUCKING DARE TOUCH THIS CODE IT TOOK ME 2 HOURS
	//wx -= 8;
	//wy += 6;


	//if (wx < 0 || wy < 0 || wx > radius * 2 || wy > radius * 2) {
//		return;
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
				if (mouseClicked) {
					this->hud->showClickMenu(&tiles[i * rad * 2 + j]);
				}
				return;
			}
		}
	}

	//if not tile and not hud
	if (mouseClicked) {
		this->hud->closeClickMenu();
	}
}
/*
new code once Ive figured it out
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
