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

    noiseScl = root["noiseScl"].asDouble();
    noiseZ = root["noiseZ"].asDouble();
    noiseEffect = root["noiseEffect"].asDouble();

    factionColour = olc::Pixel(rand() % 256, rand() % 256, rand() % 256);
    isInFaction = false;
}

//fun algorithm, makes cool shapes gonna keep for reference maybe makin planets
/*
for (uint32_t lx = 0; lx < this->radius * 2; lx++) {
    for (uint32_t ly = 0; ly < this->radius * 2; ly++) {
        int32_t rx = lx - this->radius;
        int32_t ry = ly - this->radius;
        if (rx * rx + ry * ry < this->radius * this->radius) {
            double noise = (noiseGen.GetNoise(lx * 30, ly * 30, 0) + 1) / 4 + 0.5;
            uint8_t r = colour.r / noise; //mix up *s and /s
            uint8_t g = colour.g * noise;
            uint8_t b = colour.b * noise;
            e->Draw({(signed)lx, (signed)ly}, olc::Pixel(r, g, b));
        } else {
            e->Draw({(signed)lx, (signed)ly}, olc::Pixel(0, 0, 0, 0));
        }
    }
}
*/

void Star::drawTexture(olc::PixelGameEngine *e, uint32_t scale) {
    double radius = this->radius * scale;
    buf = new olc::Sprite(radius * 2, radius * 2);
    e->SetDrawTarget(buf);
    for (uint32_t lx = 0; lx < radius * 2; lx++) {
        for (uint32_t ly = 0; ly < radius * 2; ly++) {
            int32_t rx = lx - radius;
            int32_t ry = ly - radius;
            if (rx * rx + ry * ry < radius * radius) {
                double noise = (noiseGen.GetNoise(lx * noiseScl / scale, ly * noiseScl / scale, noiseZ) + 1) / 2;
                noise = noise * noiseEffect + (1 - noiseEffect); //I think
                uint8_t r = colour.r * noise;
                uint8_t g = colour.g * noise;
                uint8_t b = colour.b * noise;
                e->Draw({(signed)lx, (signed)ly}, olc::Pixel(r, g, b));
            } else {
                e->Draw({(signed)lx, (signed)ly}, olc::Pixel(factionColour.r, factionColour.g, factionColour.b, isInFaction ? 64 : 0));
            }
        }
    }
    e->SetDrawTarget(nullptr);
    decBuf = new olc::Decal(buf);
}

void Star::draw(olc::PixelGameEngine * e, CamParams &trx, int secOffsetX, int secOffsetY) {
    if (radius * trx.zoom < 0.2 * randomDissapearChance) {
        return;
    }
    int ax = (this->x + secOffsetX) * trx.zoom + trx.tx;
    int ay = (this->y + secOffsetY) * trx.zoom + trx.ty;
    if (ax >= (WIDTH + radius) || ay >= (HEIGHT + radius) || ax <= (0 - radius) || ay <= (0 - radius)) {
        return;
    }


    if (buf == nullptr) {
        drawTexture(e, 1);
    } else if (buf->width != this->radius * 2) {
        delete decBuf;
        delete buf;
        drawTexture(e, 1);
    } else {
        if (decBuf != nullptr && buf != nullptr)
            e->DrawDecal({(float)(ax - radius * trx.zoom), (float)(ay - radius * trx.zoom)}, decBuf, {trx.zoom, trx.zoom});
        //e->FillCircle(ax, ay, this->radius * trx.zoom, this->colour);
    }
    if (isInFaction) {
        e->FillCircle(ax, ay, 32 * trx.zoom, olc::Pixel(factionColour.r, factionColour.g, factionColour.b, 64));
        e->DrawCircle(ax, ay, 32 * trx.zoom, factionColour);
    }

}

void Star::drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, CamParams &trx) {
    if (decBuf == nullptr || buf == nullptr) {
        drawTexture(e, 6);
    } else if (buf->width != this->radius * 12) {
        delete decBuf;
        delete buf;
        drawTexture(e, 6);
    } else {
        e->DrawDecal({(float)((WIDTH  / 2) * trx.zoom + trx.tx - radius * 6 * trx.zoom),
                      (float)((HEIGHT / 2) * trx.zoom + trx.ty - radius * 6 * trx.zoom)},
                      decBuf, {trx.zoom, trx.zoom});
    }
    for (Planet & p: this->planets) {
        p.theta += p.angularVelocity * fElapsedTime;
        p.rotationTheta += p.rotationalAngularVelocity * fElapsedTime;
        float ax = p.posFromStar * cos(p.theta);
        float ay = p.posFromStar * sin(p.theta);
        e->DrawCircle((WIDTH / 2) * trx.zoom + trx.tx, (HEIGHT / 2) * trx.zoom + trx.ty, p.posFromStar * trx.zoom, olc::Pixel(60, 60, 60, 200));
        p.draw(e, ax + (WIDTH / 2), ay + (HEIGHT / 2), trx);
    }
}

Planet * Star::getPlanetAt(int ax, int ay, CamParams &trx) {
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
