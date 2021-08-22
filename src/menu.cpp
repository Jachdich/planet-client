#include "menu.h"
#include "sprites.h"
#include "game.h"
#include "client.h"
#include <iostream>

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
}

bool Menu::draw(Game *e) {
    MenuComponent bg = menuComponents["background"];
    e->DrawDecal({0, 0}, bg.decal, 1.0 / (olc::vd2d)pixelsPerPixel);
    if (e->username != "" && e->password != "") {
        e->DrawStringDecal({4, 16}, "Logged in as: " + e->username);
    } else {
        e->DrawStringDecal({4, 16}, "Logged out");
    }
  
    if (e->GetKey(olc::Key::T).bPressed) {
		loadSprites();
	}

    bool just_changed = state != lastState;
    lastState = state;
    switch (state) {
	   	case MAIN: {
		    MenuComponent buttons = menuComponents["main"];
		    e->DrawDecal({0, 0}, buttons.decal, 1.0 / (olc::vd2d)pixelsPerPixel);
		    if (e->GetMouse(0).bPressed) {
		    	olc::vd2d adjustedPos = {(double)e->GetMouseX() / (double)WIDTH, (double)e->GetMouseY() / (double)HEIGHT};
				if (buttons.buttons["multiplayer"].isInside(adjustedPos)) {
					state = MULTI;
				} else if (buttons.buttons["quit"].isInside(adjustedPos)) {
					return false;
				}
		    }
		    if (e->GetKey(olc::Key::ESCAPE).bPressed) {
		        return false;
		    }
		    break;
		}
	    case MULTI: {
	    	MenuComponent buttons = menuComponents["multiplayer"];
	    	e->DrawDecal({0, 0}, buttons.decal, 1.0 / (olc::vd2d)pixelsPerPixel);
			if (e->GetMouse(0).bPressed) {
		    	olc::vd2d adjustedPos = {(double)e->GetMouseX() / (double)WIDTH, (double)e->GetMouseY() / (double)HEIGHT};
				if (buttons.buttons["server_list"].isInside(adjustedPos)) {
					state = SERVER_LIST;
				} else if (buttons.buttons["connect"].isInside(adjustedPos)) {
				    if (e->username == "" || e->password == "") {
				        state = LOGIN;
				        nextState = CONNECT;
				    } else {
					    state = CONNECT;
				    }
				} else if (buttons.buttons["back"].isInside(adjustedPos)) {
					state = MAIN;
				}
			}
			if (e->GetKey(olc::Key::ESCAPE).bPressed) {
			    state = MAIN;
			}
			break;
	    }

	    case CONNECT: {
	        if (just_changed) {
	            focus = IP_INPUT;
	        }
	    	MenuComponent buttons = menuComponents["serverconnect"];
	    	e->DrawDecal({0, 0}, buttons.decal, 1.0 / (olc::vd2d)pixelsPerPixel);
	    	getInput(ipInput, e);
	    	e->DrawStringDecal(buttons.buttons["box"].pos * olc::vd2d{(double)WIDTH, (double)HEIGHT}, ipInput, olc::Pixel(0, 0, 0), 3.0 / (olc::vd2d)pixelsPerPixel);
			if (e->GetKey(olc::Key::ESCAPE).bPressed) {
				state = MULTI;
			}
		    if (e->GetKey(olc::Key::ENTER).bPressed) {
            	e->address = ipInput;
            	e->connectToServer();
            	e->menuView = false;
            	e->galaxyView = true;
            }
			break;
	    }

	    case LOGIN: {
	        if (just_changed) {
	            focus = UNAME_INPUT;
	        }
	        MenuComponent buttons = menuComponents["login"];
	        e->DrawDecal({0, 0}, buttons.decal, 1.0 / (olc::vd2d)pixelsPerPixel);
	        olc::Decal *cursor = UIComponents["cursor"].decal;
	        olc::vi2d stringSize;
	        olc::vd2d textpos;
	        if (focus == UNAME_INPUT) {
	            getInput(unameInput, e);
	            stringSize = e->GetTextSize(unameInput);
	            textpos = buttons.buttons["username"].pos;
	        } else if (focus == PWORD_INPUT) {
	            getInput(pwordInput, e);
	            stringSize = e->GetTextSize(pwordInput);
	            textpos = buttons.buttons["password"].pos;
	        }
	        olc::vd2d pos = textpos * olc::vd2d{(double)WIDTH, (double)HEIGHT} +
                            olc::vd2d{(double)stringSize.x, 0} * (3.0 / (olc::vd2d)pixelsPerPixel) + olc::vd2d{2, -2};
            e->DrawDecal(pos, cursor);
	        e->DrawStringDecal(buttons.buttons["username"].pos * olc::vd2d{(double)WIDTH, (double)HEIGHT},
	                           unameInput, olc::Pixel(0, 0, 0), 3.0 / (olc::vd2d)pixelsPerPixel);
	        e->DrawStringDecal(buttons.buttons["password"].pos * olc::vd2d{(double)WIDTH, (double)HEIGHT},
	                           pwordInput, olc::Pixel(0, 0, 0), 3.0 / (olc::vd2d)pixelsPerPixel);

	        if (e->GetKey(olc::Key::TAB).bPressed) {
	            if (focus == UNAME_INPUT) {
	                focus = PWORD_INPUT;
	            } else {
	                focus = UNAME_INPUT;
	            }
	        }

	        if (e->GetKey(olc::Key::ENTER).bPressed) {
	            if (focus == UNAME_INPUT) {
	                focus = PWORD_INPUT;
	            } else if (focus == PWORD_INPUT) {
	                e->username = unameInput;
	                e->password = pwordInput;
	                state = nextState;
	            }
	        }
	        
	        if (e->GetKey(olc::Key::ESCAPE).bPressed) {
	            state = MULTI;
	        }
	        //once logged in, go to `nextState`
	    }
	    default: break;
    }
    return true;
}
