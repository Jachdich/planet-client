#ifndef __GUI_ITEM_H
#define __GUI_ITEM_H

#include "olcPixelGameEngine.h"
#include "MathUtils.h"

class Game;

class GUIItem{

protected:
    olc::vf2d position; 
    olc::Decal* decal;
    AABB aabb;
    const std::string text;
    bool mouse_inside;
    Game* game;
    int left_text_margin;
    int top_text_margin;

public:
    GUIItem(olc::vf2d position, olc::Decal* decal, AABB aabb, const std::string text, int left_text_margin = 5, int top_text_margin = 5);

    virtual bool draw();
    virtual void onMouseEntered();
    virtual void onMouseLeft();
    virtual void onMouseClick();

    bool clicked();

    olc::Decal* getDecal();
    void setDecal(olc::Decal* decal);

    std::string getText();
    void setText(const std::string text); 

};

#endif