#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "FastNoise.h"
#include <vector>
#include <string>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>
#include <thread>
#include <condition_variable>

#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectorcache.h"
#include "game.h"
#include "client.h"
#include <iostream>

FastNoise noiseGen;
std::mutex cache_mutex;
bool debugMode = false;
Game * app;
olc::vi2d pixelsPerPixel;
Json::Value configJson;

uint16_t WIDTH = 960;
uint16_t HEIGHT = 540;

class SurfaceRenderer : public olc::PixelGameEngine {
public:
    int lastMouseX;
    int lastMouseY;
    double totalTime = 0;
    CamParams trx = {0, 0, 1};
    Planet *planet = nullptr;
    PlanetSurface *surface = nullptr;
	SurfaceRenderer() {}

    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
    void destruct();
	bool OnUserCreate() {
	    loadSprites();

        std::string request;
        std::ifstream ifs("planet_s4.2_t2_p0.json");
        std::ostringstream sstr;
        sstr << ifs.rdbuf();
        request = sstr.str();
        ifs.close();
        std::cout << request << "\n";
	    Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value root;
        std::string errors;

        std::cout << reader->parse(
            request.c_str(),
            request.c_str() + request.size(),
            &root,
            &errors
        ) << "\n";
        delete reader;
	    planet = new Planet(root, 0);
	    //surface = new PlanetSurface()
	    return true;
	}
	bool OnUserUpdate(float fElapsedTime) {
	    if (GetMouse(1).bPressed) {
            lastMouseX = GetMouseX();
            lastMouseY = GetMouseY();
        }

        if (GetMouse(1).bHeld) {
            int offX = GetMouseX() - lastMouseX;
            int offY = GetMouseY() - lastMouseY;
            trx.tx += offX;
            trx.ty += offY;
            lastMouseX = GetMouseX();
            lastMouseY = GetMouseY();
        }
        int count = GetMouseWheel();
        if (count != 0) {
            zoom(-count);
        }
        planet->draw(this, 0, 0, trx);
	    totalTime += fElapsedTime;
        trx.animationStage = totalTime * 10;
        return true;
	}
    void zoom(int count) {
        if (trx.zoom >= 8 && count < 0) {
            return;
        }
        if (trx.zoom <= 0.18 && count > 0) {
            return;
        }
        trx.tx -= GetMouseX();
        trx.ty -= GetMouseY();
        double delta = count < 0 ? 1.05 : count > 0 ? 1.0/1.05 : 1.0;
        trx.zoom *= delta;
        trx.tx *= delta;
        trx.ty *= delta;
        trx.tx += GetMouseX();
        trx.ty += GetMouseY();
    }
    
	std::vector<int> getCurrentPlanetsurfaceLocator();
};

int main() {

	pixelsPerPixel = {2, 2};
	
    noiseGen.SetNoiseType(FastNoise::SimplexFractal);
    noiseGen.SetFractalType(FastNoise::FBM);
    noiseGen.SetFractalOctaves(5);
    noiseGen.SetFractalLacunarity(2);
    noiseGen.SetFractalGain(0.5);
    
	SurfaceRenderer *app = new SurfaceRenderer();
	if (app->Construct(1920/2, 1080/2, 2, 2, false))
		app->Start();
	else
	    std::cout << "what\n";
    return 0;
}
