#ifndef __PLANET_H
#define __PLANET_H
#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planetsurface.h"

class Planet {
public:
    int radius;
    std::vector<olc::Pixel> texture;
    olc::Sprite *surf = nullptr;
    olc::Decal  *dec;
    int posFromStar;
    double theta;
    double rotationTheta;
    double rotationalAngularVelocity;
    double angularVelocity;
    int posInStar;
	    
    PlanetSurface surface;
    
    Planet();
    Planet(Json::Value res, int posInStar);
    void draw(olc::PixelGameEngine * e, double x, double y, CamParams &trx);
    void drawSurface(olc::PixelGameEngine * e, CamParams &trx);
};

#endif
