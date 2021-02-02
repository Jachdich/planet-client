#ifndef __GAME_H
#define __GAME_H
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
//
//#include "client.h"
#include "MathUtils.h"
#include "network.h"
#include "sector.h"
#include "sectorcache.h"
#include "sprites.h"
#include "star.h"
#include "menu.h"
#include "gui_item.h"
#include "scene.h"
#include "main_menu.h"

class Game : public olc::PixelGameEngine {
public:
    Scene* current_scene;
    int lastMouseX;
    int lastMouseY;
    bool test;
    CamParams galaxyTrx;
    CamParams trx;
    Star * selectedStar = nullptr;
    Planet * selectedPlanet = nullptr;
    Sector * lastClickedSector = nullptr;
    bool galaxyView = false;
    bool planetView = false;
    bool starView = false;
    bool menuView = true;
    Menu menu;
    SectorCache map;
    ClientNetwork client;
    std::string address;
    
	Game();
    Game(int argc, char ** argv);

    void connectToServer();

    olc::vd2d getLastMousePos(){
        return olc::vd2d((double)lastMouseX, (double)lastMouseY);
    }
    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
    void destruct();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    void zoom(int count);
    void mousePressed();
    void changeScene(Scene* scene);

	std::vector<int> getCurrentPlanetsurfaceLocator();
};

#endif
