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
	LOGIN,
	CONNECT,
};

enum Focus {
    IP_INPUT,
    UNAME_INPUT,
    PWORD_INPUT,
};

class Menu {
private:
	std::string ipInput = "";
	std::string unameInput = "";
	std::string pwordInput = "";
	Focus focus = IP_INPUT;
    MenuState state;
    MenuState lastState;
    MenuState nextState;
public:
    Menu();
    bool draw(Game * e);    
};

#endif
