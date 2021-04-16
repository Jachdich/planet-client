#ifndef __SCENE_H
#define __SCENE_H

#include "guiItem.h"

#include "sprites.h"

class Game;

class Scene{
private:
    std::unique_ptr<olc::Sprite> background_sprite;
    std::unique_ptr<olc::Decal> background;
public:
    Scene(const std::string background_path = "textures/backgrounds/background_default.png");
    ~Scene();

    std::vector<GUIItem*> items;

    virtual bool draw(float fElapsedTime);

};


#endif
