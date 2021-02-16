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
#include "guiItem.h"
#include "scene.h"
#include "mainMenu.h"
#include "multiplayerMenu.h"
#include "connectIpMenu.h"
#include "galaxyview.h"
#include "starview.h"
#include "planetview.h"

class Game : public olc::PixelGameEngine {

public: //scene
    std::unique_ptr<MultiplayerMenu> multiPlayerMenu;
    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<ConnectToIpMenu> connectToIPMenu;
    std::unique_ptr<Galaxy> galaxy;
    std::unique_ptr<StarView> starViewScene;
    std::unique_ptr<PlanetView> planetViewScene;
public:
    bool exit = false;
    Scene* currentScene;
    int lastMouseX;
    int lastMouseY;
    bool test;
    CamParams galaxyTrx;
    CamParams trx;
    Star * selectedStar = nullptr;
    Planet * selectedPlanet = nullptr;
    Sector * lastClickedSector = nullptr;
    bool menuView = true;
    SectorCache map;
    ClientNetwork client;
    std::string address;
    
	Game();
    Game(int argc, char ** argv);

    bool connectToServer();

    olc::vd2d getLastMousePos(){
        return olc::vd2d((double)lastMouseX, (double)lastMouseY);
    }
    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    void zoom(int count);
    void changeScene(Scene* scene);
    void updateCamParams();

	std::vector<int> getCurrentPlanetsurfaceLocator();
};

#endif
