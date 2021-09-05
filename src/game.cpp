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
        connectImmediately = true;
    }
}

void Game::destruct() {

}

bool Game::OnUserCreate() {
    loadSprites();
    if (connectImmediately) {
        connectToServer();
        galaxyView = true;
        menuView = false;
    }
    return true;
}

std::vector<int> Game::getCurrentPlanetsurfaceLocator() {
	return std::vector<int>{lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar};
}

void Game::mousePressed(uint32_t x, uint32_t y) {
    if (galaxyView) {
        Sector * s = map.getSectorAt(floor((x - trx.tx) / trx.zoom / 256),
                                     floor((y - trx.ty) / trx.zoom / 256));
        lastClickedSector = s;
 
        Star * st = s->getStarAt(x, y, trx);

        if (st != nullptr) {
            this->selectedStar = st;
            this->starView = true;
            this->galaxyView = false;
            galaxyTrx = trx;
            trx = {0, 0, 1};
        }
    } else if (starView) {
        Planet * p = selectedStar->getPlanetAt(x, y, trx);
        if (p != nullptr) {
            this->selectedPlanet = p;
            galaxyView = false;
            starView = false;
            planetView = true;
            if (selectedPlanet->surface == nullptr || (!selectedPlanet->surface->generated && !selectedPlanet->surface->requested)) {
        		selectedPlanet->loadSurface(lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar);
        	}
            trx = {0, 0, 1};
        }
    }
}

void Game::connectToServer() {
    client.connect(address, 5555, &map, username, password);
}

bool Game::OnUserUpdate(float fElapsedTime) {
    /*if (planetView) {
        Clear(olc::Pixel(50, 100, 160));
    } else {
	}*/
	Clear(olc::BLACK);
	if (menuView) {
	    return menu.draw(this);
	}

    if (galaxyView) {
        map.draw(this, trx);
    } else if (starView) {
        selectedStar->drawWithPlanets(this, fElapsedTime, trx);
    } else if (planetView) {
    	if (selectedPlanet->surface->generated) {
    	    selectedPlanet->surface->data->updateTimers(fElapsedTime);
			selectedPlanet->surface->mouseOver(GetMouseX(), GetMouseY(), GetMouse(0).bPressed, GetMouse(0).bHeld, GetMouse(1).bPressed, trx);
    		selectedPlanet->drawSurface(this, trx);
    	}
    }

    if (GetKey(olc::Key::W).bHeld) trx.ty += fElapsedTime * 500;
    if (GetKey(olc::Key::A).bHeld) trx.tx += fElapsedTime * 500;
    if (GetKey(olc::Key::S).bHeld) trx.ty -= fElapsedTime * 500;
    if (GetKey(olc::Key::D).bHeld) trx.tx -= fElapsedTime * 500;
    if (GetKey(olc::Key::Q).bHeld) trx.zoom *= (1 + fElapsedTime * 0.6);
    if (GetKey(olc::Key::E).bHeld) trx.zoom *= (1 - fElapsedTime * 0.6);

    int count = GetMouseWheel();
    if (count != 0) {
        zoom(-count);
    }

    if (GetKey(olc::Key::ENTER).bPressed) {
        mousePressed(WIDTH/2, HEIGHT/2);
    }

    if (GetMouse(0).bPressed) {
        mousePressed(GetMouseX(), GetMouseY());
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

    if (GetKey(olc::Key::L).bPressed) {
        std::cout << "L pressed\n";
    }

	if (GetKey(olc::Key::T).bPressed) {
		loadSprites();
	}

    if (GetKey(olc::Key::ESCAPE).bPressed) {
        if (planetView) {
            if (selectedPlanet->surface->hud->popupMessage != "") {
                selectedPlanet->surface->hud->popupMessage = "";
            } else if (selectedPlanet->surface->hud->ddmenu != nullptr) {
                selectedPlanet->surface->hud->closeClickMenu();
            } else if (selectedPlanet->surface->hud->selectedAction != TaskType::NONE) {
                selectedPlanet->surface->hud->selectedAction = TaskType::NONE;
            } else {
                starView = true;
                planetView = false;
                galaxyView = false;
                trx = {0, 0, 1};
                std::vector<int> surface = getCurrentPlanetsurfaceLocator();
                Json::Value root;
    		    root["request"] = "unloadSurface";
    		    root["secX"] = surface[0];
    		    root["secY"] = surface[1];
    		    root["starPos"] = surface[2];
    		    root["planetPos"] = surface[3];
    		    client.sendRequest(root);
    		    selectedPlanet->unloadSurface();
		    }
        } else if (starView) {
            starView = false;
            galaxyView = true;
            trx = galaxyTrx;
            this->selectedStar = nullptr;
        } else if (galaxyView) {
            return false;
        }
    }

	if (GetKey(olc::Key::F3).bPressed) {
		debugMode = !debugMode;
	}

	if (debugMode) {
		DrawStringDecal({0, 0}, std::to_string(map.secs.size()), olc::Pixel(255, 255, 255));
		DrawStringDecal({0, 10}, std::to_string(fElapsedTime * 1000), olc::Pixel(255, 255, 255));
		DrawStringDecal({0, 20}, std::to_string(1.0 / fElapsedTime), olc::Pixel(255, 255, 255));
	}
	SetPixelMode(olc::Pixel::ALPHA);

    DrawLine({WIDTH/2 - 10, HEIGHT/2}, {WIDTH/2 + 10, HEIGHT/2}, olc::WHITE);
    DrawLine({WIDTH/2, HEIGHT/2 - 10}, {WIDTH/2, HEIGHT/2 + 10}, olc::WHITE);

    totalTime += fElapsedTime;
    trx.animationStage = totalTime * 10;

    return true;
}

void Game::zoom(int count) {
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
