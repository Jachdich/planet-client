#include "planetsurface.h"

#include <stdlib.h>
#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planet.h"
#include "sprites.h"

olc::Pixel PlanetSurface::getTint(int x, int y, Planet * parent) {
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
    return olc::Pixel(r * 0.9, g * 0.9, b * 0.9);
}

void PlanetSurface::drawTile(int ax, int ay, int thing, olc::PixelGameEngine * e, CamParams trx, Planet * p) {
    //e->DrawDecal({ax * 64 + trx.tx, ay * 64 + trx.ty}, testDecal, {1.0f, 1.0f}, getTint(ax, ay, p));
    e->DrawDecal({(ax * 64 - ay * 64) * trx.zoom + trx.tx, ((ax * 64 + ay * 64) / 2) * trx.zoom + trx.ty}, decals[0], {trx.zoom, trx.zoom}, getTint(ax, ay, p));
    
    if (thing == 1) {
        e->DrawDecal({(ax * 64 - ay * 64) * trx.zoom + trx.tx, ((ax * 64 + ay * 64) / 2) * trx.zoom + trx.ty}, decals[thing], {trx.zoom, trx.zoom}, getTint(ax, ay, p));
    }
}

void PlanetSurface::draw(olc::PixelGameEngine * e, CamParams trx, Planet * p) {
    srand(1000);
    for (int i = 0; i < p->radius * 2; i++) {
        for (int j = 0; j < p->radius * 2; j++) {
            int ia = i - p->radius;
            int ja = j - p->radius; //I see you there hacking into my VNC which has an insecure password. Please stop swapping j to i.
            if ((ia * ia + ja * ja) >= (p->radius * p->radius)) continue;
            drawTile(i, j, tiles[i * p->radius * 2 + j], e, trx, p);
        }
    }
}

PlanetSurface::PlanetSurface() {
}


PlanetSurface::PlanetSurface(Json::Value root) {

	tiles.reserve(root["rad"].asInt() * 2);
	for (int i = 0; i < root["rad"].asInt() * 2; i++) {
		for (int j = 0; j < root["rad"].asInt() * 2; j++)
		tiles.push_back(root["tiles"][i + j * root["rad"].asInt() * 2].asInt());
	}
	generated = true;
	requested = false;
}
