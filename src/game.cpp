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

    std::string address = "127.0.0.1";
    if (args.size() > 1) {
        address = args[1];
    }

    client.connect(address, 5555, &map);
}

void Game::destruct() {

}

bool Game::OnUserCreate() {
    loadSprites();

    return true;
}

std::vector<int> Game::getCurrentPlanetsurfaceLocator() {
	return std::vector<int>{lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar};
}

bool Game::OnUserUpdate(float fElapsedTime) {
    //if (planetView) {
    //    Clear(selectedPlanet->baseColour);
    //} else {
	Clear(olc::BLACK);
    //}

    if (galaxyView) {
        map.draw(this, trx);
    } else if (starView) {
        selectedStar->drawWithPlanets(this, fElapsedTime, trx);
    } else if (planetView) {
    	if (selectedPlanet->surface->generated) {
            if (!selectedPlanet->surface->threadStarted) {
                this->selectedPlanet->surface->startThread();
            }
			selectedPlanet->surface->mouseOver(GetMouseX(), GetMouseY(), GetMouse(0).bPressed, GetMouse(0).bHeld, trx);
    		selectedPlanet->drawSurface(this, trx);
    	}
    }

    int count = GetMouseWheel();
    if (count != 0) {
        zoom(-count);
    }

    if (GetMouse(0).bPressed) {
        if (galaxyView) {
            Sector * s = map.getSectorAt(floor((GetMouseX() - trx.tx) / trx.zoom / 256), floor((GetMouseY() - trx.ty) / trx.zoom / 256));
            lastClickedSector = s;
            //std::cout << "clicked sector " << s->x << " " << s->y << "\n";
            //std::cout << s->requested << " " << s->x << " " << s->y << "\n";
            Star * st = s->getStarAt(GetMouseX(), GetMouseY(), trx);
                    // (GetMouseX() - trx.tx) / trx.zoom - floor((GetMouseX() - trx.tx) / trx.zoom / 256) * 256,
                    // (GetMouseY() - trx.ty) / trx.zoom - floor((GetMouseY() - trx.ty) / trx.zoom / 256) * 256);
            if (st != nullptr) {
                this->selectedStar = st;
                this->starView = true;
                this->galaxyView = false;
                galaxyTrx = trx;
                trx = {0, 0, 1};
            }
        } else if (starView) {
            Planet * p = selectedStar->getPlanetAt(GetMouseX(), GetMouseY(), trx);
            if (p != nullptr) {
                this->selectedPlanet = p;
                galaxyView = false;
                starView = false;
                planetView = true;
                if (!selectedPlanet->surface->generated and !selectedPlanet->surface->requested) {
            		selectedPlanet->loadSurface(lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar);
            	}
                trx = {0, 0, 1};
            }
        }
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
        //std::cout << "L pressed\n";
        std::cout << this->selectedPlanet->surface->data->toJSON() << "\n";
    }

	if (GetKey(olc::Key::T).bPressed) {
		loadSprites();
	}

    if (GetKey(olc::Key::ESCAPE).bPressed) {
        if (planetView) {
            this->selectedPlanet->surface->cleanUpThread();
            starView = true;
            planetView = false;
            galaxyView = false;
            trx = {0, 0, 1};
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
	}

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
