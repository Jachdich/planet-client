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
#include "menu.h"

class Game : public olc::PixelGameEngine {
public:
    int lastMouseX;
    int lastMouseY;
    CamParams galaxyTrx;
    CamParams trx;
    Star * selectedStar = nullptr;
    Planet * selectedPlanet = nullptr;
    Sector * lastClickedSector = nullptr;
    bool galaxyView = false;
    bool planetView = false;
    bool starView = false;
    bool menuView = true;
    double totalTime = 0;

    bool connectImmediately = false;
    
    Menu menu;
    SectorCache map;
    ClientNetwork client;
    std::string address;
    
	Game();
    Game(int argc, char ** argv);

    void connectToServer();
    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
    void destruct();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    void zoom(int count);
    void mousePressed();
	std::vector<int> getCurrentPlanetsurfaceLocator();
};

#endif
