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
#include "multiplayer_menu.h"
#include "connect_ip_menu.h"
#include "galaxyview.h"
#include "starview.h"

class Game : public olc::PixelGameEngine {

public: //scene
    std::unique_ptr<MultiplayerMenu> multiPlayerMenu;
    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<ConnectToIpMenu> connectToIPMenu;
    std::unique_ptr<Galaxy> galaxy;
    std::unique_ptr<StarView> starViewScene;
public:
    bool exit = false;
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

    bool connectToServer();

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
