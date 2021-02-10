#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include "scene.h"
#include "sprites.h"

using namespace GUI;

class MainMenu : public Scene{

public:
    
    MainMenu();

    bool draw() override;
    void onGUIItemClicked(const std::string& name) override;
    void checkForInput() override;

};

#endif