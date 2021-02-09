#ifndef __MULTIPLAYER_MENU_H
#define __MULTIPLAYER_MENU_H

#include "scene.h"

class MultiplayerMenu : public Scene{

public:
    MultiplayerMenu();
        
    void onGUIItemClicked(const std::string& name) override;

};

#endif