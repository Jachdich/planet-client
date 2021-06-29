#include "sector.h"

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "star.h"

Sector::Sector() {}

void Sector::setRequested() {
    requested = true;
}

Sector::Sector(Json::Value root) {
    this->x = root["x"].asUInt();
    this->y = root["y"].asUInt();
    this->r = root["r"].asInt();
    this->generated = true;
    this->requested = false;
    this->numStars = root["numStars"].asInt();
    for (int i = 0; i < numStars; i++) {
        stars.push_back(Star(root["stars"][i], (int)stars.size()));
    }

}

Star * Sector::getStarAt(int ax, int ay, CamParams &trx) {
    int wx = (ax - trx.tx) / trx.zoom;
    int wy = (ay - trx.ty) / trx.zoom;
    int bx = wx - r * x;
    int by = wy - r * y;
    for (int i = 0; i < numStars; i++) {
        int dx = bx - stars[i].x;
        int dy = by - stars[i].y;
        if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
            return &this->stars[i];
        }
    }
    return nullptr;
}

void Sector::draw(olc::PixelGameEngine * e, CamParams &trx) {
	if (debugMode) {
		e->DrawRect(this->x * this->r * trx.zoom + trx.tx, this->y * this->r * trx.zoom + trx.ty, r * trx.zoom, r * trx.zoom, olc::Pixel(255, 255, 255));
        e->DrawStringDecal({this->x * this->r * trx.zoom + (int)trx.tx, this->y * this->r * trx.zoom + (int)trx.ty}, std::to_string(this->x) + ", " + std::to_string(this->y), olc::WHITE);
	}
    for (int i = 0; i < this->numStars; i++) {
        this->stars[i].draw(e, trx, x * r, y * r);
	}
}
