#ifndef __SCENE_H
#define __SCENE_H

#include "gui_item.h"

#include "sprites.h"

class Game;

class Scene{
private:
    std::unique_ptr<olc::Sprite> background_sprite;
    std::unique_ptr<olc::Decal> background;
public:
    Scene(){}
    Scene(const std::string background_path);
    ~Scene();

    std::vector<GUIItem*> items;

    bool draw();

    void setBackground(olc::Decal* dec){
        //this->background.reset(dec);
    }

};

#endif