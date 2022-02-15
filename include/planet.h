#ifndef __PLANET_H
#define __PLANET_H
#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planetsurface.h"

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    std::vector<double> generationChances;
    std::vector<olc::Pixel> generationColours;
    std::vector<int> generationZValues;
    std::vector<double> generationNoise;
    olc::Pixel baseColour;
	olc::Sprite * surf = nullptr;
	olc::Decal  * dec;
    int posFromStar;
    double theta;
	double rotationTheta;
	double rotationalAngularVelocity;
    double angularVelocity;
    int posInStar;
    int32_t seaLevel;
	    
    PlanetSurface * surface = nullptr;
    
    Planet();
    Planet(Json::Value res, int posInStar);
    void loadSurface(int secX, int secY, int starPos, int planetPos);
    void unloadSurface();
    void draw(olc::PixelGameEngine * e, double x, double y, CamParams &trx);
    void drawSurface(olc::PixelGameEngine * e, CamParams &trx);
};

#endif
