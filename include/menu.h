#ifndef __MENU_H
#define __MENU_H
class Game;

class Menu {
private:
    bool main = true;
    bool multi = false;
    bool load = false;
    bool newSave = false;
    bool options = false;

public:
    Menu();
    void draw(Game * e);    
};

#endif