#include "menu.h"
#include "sprites.h"
#include "game.h"
#include "client.h"

Menu::Menu() {

}

void getInput(std::string &inp, olc::PixelGameEngine *m_pge) {
	bool bShift = m_pge->GetKey(olc::Key::SHIFT).bHeld;
	//bool bClick = false;
    // Handle standard Characters
    for (int i = 1; i < 27; i++)
        if (m_pge->GetKey((olc::Key)i).bPressed) {
            inp += ((bShift ? (char)(i + 64) : (char)(i + 96)));// bClick = true;
        }

    // Handle Keyboard Numbers
    for (int i = 27; i < 37; i++)
        if (m_pge->GetKey((olc::Key)i).bPressed) {
            inp += ((char)(i + 48 - 27));// bClick = true;
        }

    // Handle Numpad Numbers
    for (int i = 69; i < 79; i++)
        if (m_pge->GetKey((olc::Key)i).bPressed) {
            inp += ((char)(i + 48 - 69));// bClick = true;
    	}

    if (m_pge->GetKey(olc::Key::PERIOD).bPressed) inp += ".";

    if (m_pge->GetKey(olc::Key::BACK).bPressed && !inp.empty()) {
        inp.pop_back();// bClick = true;
    }

    if (inp.size() > 15) {
        inp.pop_back();
    }

    if (m_pge->GetKey(olc::Key::ENTER).bPressed) {
    	app->address = inp;
    	app->connectToServer();
    	app->menuView = false;
    	app->galaxyView = true;
    }
}

bool Menu::draw(Game *e) {
    MenuComponent bg = menuComponents["background"];
    e->DrawDecal({0, 0}, bg.decal, 1.0f / pixelsPerPixel);
    if (e->GetKey(olc::Key::T).bPressed) {
		loadSprites();
	}

    switch (state) {
	   	case MAIN: {
		    MenuComponent buttons = menuComponents["main"];
		    e->DrawDecal({0, 0}, buttons.decal, 1.0f / pixelsPerPixel);
		    if (e->GetMouse(0).bPressed) {
		    	olc::vd2d adjustedPos = {(double)e->GetMouseX() / (double)WIDTH, (double)e->GetMouseY() / (double)HEIGHT};
				if (buttons.buttons["multiplayer"].isInside(adjustedPos)) {
					state = MULTI;
				} else if (buttons.buttons["quit"].isInside(adjustedPos)) {
					return false;
				}
		    }
		    break;
		}
	    case MULTI: {
	    	MenuComponent buttons = menuComponents["multiplayer"];
	    	e->DrawDecal({0, 0}, buttons.decal, 1.0f / pixelsPerPixel);
			if (e->GetMouse(0).bPressed) {
		    	olc::vd2d adjustedPos = {(double)e->GetMouseX() / (double)WIDTH, (double)e->GetMouseY() / (double)HEIGHT};
				if (buttons.buttons["server_list"].isInside(adjustedPos)) {
					state = SERVER_LIST;
				} else if (buttons.buttons["connect"].isInside(adjustedPos)) {
					state = CONNECT;
				} else if (buttons.buttons["back"].isInside(adjustedPos)) {
					state = MAIN;
				}
			}
			break;
	    }

	    case CONNECT: {
	    	MenuComponent buttons = menuComponents["serverconnect"];
	    	e->DrawDecal({0, 0}, buttons.decal, 1.0f / pixelsPerPixel);
	    	getInput(ipInput, e);
	    	e->DrawStringDecal(buttons.buttons["box"].pos * olc::vd2d{(double)WIDTH, (double)HEIGHT}, ipInput, olc::Pixel(0, 0, 0), 3.0f / pixelsPerPixel);
			if (e->GetKey(olc::Key::ESCAPE).bPressed) {
				state = MULTI;
			}
			//if (e->GetMouse(0).bPressed) {
		    	//olc::vd2d adjustedPos = {(double)e->GetMouseX() / (double)WIDTH, (double)e->GetMouseY() / (double)HEIGHT};
			//}
			break;
	    }
	    default: break;
    }
    return true;
}
