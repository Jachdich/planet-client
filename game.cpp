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

Game::Game(int argc, char ** argv) : io_context(), sock(io_context), map(&sock) {
    sAppName = "Example";
    for (int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }
}

void Game::destruct() {
    
}

bool Game::OnUserCreate() {
	renderingLayer = CreateLayer();
	HUDLayer = 0;
	
	SetDrawTarget(HUDLayer);
	Clear(olc::BLANK);
	SetDrawTarget(renderingLayer);
	
	EnableLayer(renderingLayer, true);
	//EnableLayer(HUDLayer, true);
	
    std::string address = "127.0.0.1";
    if (args.size() > 1) {
        address = args[1];
    }
    
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(address, "5555");
    
    asio::connect(sock, endpoints);
    std::cout << "a\n";
    std::thread(handleNetwork, &sock, &map).detach();
    std::cout << "b\n";
    loadSprites();
      
    return true;
}

bool Game::OnUserUpdate(float fElapsedTime) {
	SetDrawTarget(renderingLayer);
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
    	if (!selectedPlanet->surface->generated and !selectedPlanet->surface->requested) {
    		//std::cout << "thing" << posInSector << " " << lastClickedSector->stars.size() << " " << posInStar << " " << selectedStar->planets.size() << "\n";

    		selectedPlanet->loadSurface(lastClickedSector->x, lastClickedSector->y, selectedStar->posInSector, selectedPlanet->posInStar);
    	}
    	if (selectedPlanet->surface->generated) {
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
            Sector * s = map.getSectorAt(floor((GetMouseX() - trx.tx) / 256), floor((GetMouseY() - trx.ty) / 256));
            lastClickedSector = s;
            std::cout << "clicked sector " << s->x << " " << s->y << "\n";
            //std::cout << s->requested << " " << s->x << " " << s->y << "\n";
            Star * st = s->getStarAt(
                    (GetMouseX() - trx.tx) / trx.zoom - floor((GetMouseX() - trx.tx) / trx.zoom / 256) * 256,
                    (GetMouseY() - trx.ty) / trx.zoom - floor((GetMouseY() - trx.ty) / trx.zoom / 256) * 256);
            if (st != nullptr) {
                this->selectedStar = st;
                this->starView = true;
                this->galaxyView = false;
                galaxyTrx = trx;
                trx = {0, 0, 1};
            }
        }
        
        if (starView) {
            Planet * p = selectedStar->getPlanetAt(GetMouseX(), GetMouseY(), trx);
            if (p != nullptr) {
                this->selectedPlanet = p;
                galaxyView = false;
                starView = false;
                planetView = true;
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
        std::cout << "L pressed\n";
    }
	
	if (GetKey(olc::Key::T).bPressed) {
		loadSprites();
	}

    if (GetKey(olc::Key::ESCAPE).bPressed) {
        if (planetView) {
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
	
    if (netRequests.size() > 0) {
        std::lock_guard<std::mutex> lock(netq_mutex);
        netq.notify_all();
    }
	
    return true;
}

void Game::zoom(int count) {
    trx.tx -= GetMouseX();
    trx.ty -= GetMouseY();
    double delta = count < 0 ? 1.05 : count > 0 ? 1.0/1.05 : 1.0;
    trx.zoom *= delta;
    trx.tx *= delta;
    trx.ty *= delta;
    trx.tx += GetMouseX();
    trx.ty += GetMouseY();
}
