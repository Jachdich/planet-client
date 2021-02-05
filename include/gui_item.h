#ifndef __GUI_ITEM_H
#define __GUI_ITEM_H

#include "olcPixelGameEngine.h"
#include "MathUtils.h"

class Game;

class GUIItem{

protected:
    std::unique_ptr<olc::Sprite> sprite;
    std::unique_ptr<olc::Decal> decal;
    olc::vf2d position; 
    const std::string text;
    bool mouse_inside;
    Game* game;
    float size;
    int left_text_margin;
    int top_text_margin;
    AABB aabb;

public:
    bool area_clicked;
    GUIItem(olc::vf2d position, const std::string decal_path, const std::string text, float size = 1.0, int left_text_margin = 5, int top_text_margin = 5);
    virtual ~GUIItem();

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