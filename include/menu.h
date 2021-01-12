#ifndef __MENU_H
#define __MENU_H
#include <string>

class Game;

enum MenuState {
	MAIN,
	MULTI,
	SINGLE,
	OPTIONS,
	SERVER_LIST,
	CONNECT,
};

class Menu {
private:
	std::string ipInput = "";
    MenuState state;
public:
    Menu();
    bool draw(Game * e);    
};

#endif
