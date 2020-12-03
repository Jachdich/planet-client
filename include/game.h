#ifndef __GAME_H
#define __GAME_H
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

//#include "client.h"
#include "network.h"
#include "sector.h"
#include "sectorcache.h"
#include "sprites.h"
#include "star.h"

class Game : public olc::PixelGameEngine {
public:
    std::vector<std::string> args;
    int lastMouseX;
    int lastMouseY;
    CamParams galaxyTrx;
    CamParams trx;
    Star * selectedStar = nullptr;
    Planet * selectedPlanet = nullptr;
    Sector * lastClickedSector = nullptr;
    bool galaxyView = true;
    bool planetView = false;
    bool starView = false;
    SectorCache map;
    ClientNetwork client;
    
	Game();
    Game(int argc, char ** argv);
    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
    void destruct();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    void zoom(int count);
	std::vector<int> getCurrentPlanetsurfaceLocator();
};

#endif
