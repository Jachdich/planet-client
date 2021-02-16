#include "game.h"

#include <asio.hpp>
#include <thread>

#include "client.h"
#include "helperfunctions.h"
#include "network.h"
#include "olcPixelGameEngine.h"
#include "sector.h"
#include "sectorcache.h"
#include "sprites.h"
#include "star.h"
#include "planetdata.h"

/*
Client: "fell tree <surfaceLocator> <coords>"
Server: "time <surfacelocator> <coords> 5"
Server: "time <surfacelocator> <coords> 4"
...
Server: "time <surfaceLocator> <coords> 1"
Server: "update item wood + 1"
Server: "changeTime <surfaceLocator> <coords> grass"
*/

Game::Game(int argc, char ** argv) : map() {
    sAppName = "Planet Game";

    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }

    address = "127.0.0.1";
    if (args.size() > 1) {
        address = args[1];
    }
}

bool Game::OnUserCreate() {
    loadSprites();
    /*
    connectToServer();
    galaxyView = true;
    menuView = false;
	*/
    mainMenu = std::make_unique<MainMenu>();
    multiPlayerMenu = std::make_unique<MultiplayerMenu>();
    connectToIPMenu = std::make_unique<ConnectToIpMenu>();
    galaxy = std::make_unique<Galaxy>();
    starViewScene = std::make_unique<StarView>();
    planetViewScene = std::make_unique<PlanetView>();
    currentScene = mainMenu.get();
    return true;
}

std::vector<int> Game::getCurrentPlanetsurfaceLocator() {
	return std::vector<int>{lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar};
}

bool Game::connectToServer() {
    if(client.connect(address, 5555, &map)) return true;
    else return false;
}

bool Game::OnUserUpdate(float fElapsedTime) {
	Clear(olc::BLACK); 

    updateCamParams();

    if(currentScene != NULL){currentScene->fElapsedTime = fElapsedTime; currentScene->draw();}
    if(exit) return false;

    return true;
}

void Game::zoom(int count) {
    if (trx.zoom >= 8 && count < 0) {
        return;
    }
    if (trx.zoom <= 0.38 && count > 0) {
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

void Game::changeScene(Scene* scene){
    this->currentScene = scene;
}

void Game::updateCamParams(){
    int count = GetMouseWheel();
    if (count != 0) {
        zoom(-count);
    }
    
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

}