#ifndef __STAR_H
#define __STAR_H

#include <jsoncpp/json/json.h>

#include "olcPixelGameEngine.h"
#include "planet.h"

class Star {
public:
    double radius;
    olc::Pixel colour;
    std::vector<Planet> planets;
    float x = 0;
    float y = 0;
    int num = 0;
    bool selected;
    int randomDissapearChance;
    int posInSector;

    double noiseZ;
    double noiseScl;
    double noiseEffect;

    olc::Pixel factionColour;
    bool isInFaction;

    olc::Sprite *buf = nullptr;
    olc::Decal *decBuf = nullptr;
    
    Star();
    Star(Json::Value root, int posInSector);
    void addPlanet(Json::Value root);
    void drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, CamParams &trx);
    void draw(olc::PixelGameEngine * e, CamParams &trx, int secOffsetX, int secOffsetY);
    void select();
    void drawTexture(olc::PixelGameEngine *e, uint32_t scale);
    Planet * getPlanetAt(int x, int y, CamParams &trx);
};
#endif
