#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include "scene.h"
#include "sprites.h"

class MainMenu : public Scene{

public:
    
    MainMenu();

    bool draw() override;

};

#endif