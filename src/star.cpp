#include "star.h"
#include <stdlib.h>
#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planet.h"
#include "sector.h"

Star::Star() {}

Star::Star(Json::Value root, int posInSector) {
    x = root["x"].asInt();
    y = root["y"].asInt();
    num = root["num"].asInt();
    int col = root["colour"].asInt();
    colour = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    radius = root["radius"].asInt();
    for (int i = 0; i < num; i++) {
        planets.push_back(Planet(root["planets"][i], (int)planets.size()));
    }
    this->selected = false;
    randomDissapearChance = 10 - (radius - 4);
    this->posInSector = posInSector;
}

void Star::draw(olc::PixelGameEngine * e, CamParams trx, int secOffsetX, int secOffsetY) {
    if (radius * trx.zoom < 0.2 * randomDissapearChance) {
        return;
    }
    int ax = (this->x + secOffsetX) * trx.zoom + trx.tx;
    int ay = (this->y + secOffsetY) * trx.zoom + trx.ty;
    if (ax >= (WIDTH + radius) || ay >= (HEIGHT + radius) || ax <= (0 - radius) || ay <= (0 - radius)) {
        return;
    }
    e->FillCircle(ax, ay, this->radius * trx.zoom, this->colour);
}

void Star::drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, CamParams trx) {
    e->FillCircle((WIDTH / 2) * trx.zoom + trx.tx, (HEIGHT / 2) * trx.zoom + trx.ty, this->radius * 6 * trx.zoom, this->colour);
    for (Planet & p: this->planets) {
        p.theta += p.angularVelocity * fElapsedTime;
		p.rotationTheta += p.rotationalAngularVelocity * fElapsedTime;
        float ax = p.posFromStar * cos(p.theta);
        float ay = p.posFromStar * sin(p.theta);
        e->DrawCircle((WIDTH / 2) * trx.zoom + trx.tx, (HEIGHT / 2) * trx.zoom + trx.ty, p.posFromStar * trx.zoom, olc::Pixel(60, 60, 60, 200));
        p.draw(e, ax + (WIDTH / 2), ay + (HEIGHT / 2), trx);
    }
}

Planet * Star::getPlanetAt(int ax, int ay, CamParams trx) {
    int x = (ax - trx.tx) / trx.zoom;
    int y = (ay - trx.ty) / trx.zoom;
    for (Planet & p: this->planets) {
        float dx = x - (p.posFromStar * cos(p.theta) + WIDTH / 2);
        float dy = y - (p.posFromStar * sin(p.theta) + HEIGHT / 2);
        if (dx * dx + dy * dy < p.radius * p.radius) {
            return &p;
        }
    }
    return nullptr;
}

void Star::select() {
    this->selected = true;
}
