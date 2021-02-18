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
    std::string text;
    Game* game;
    float size;
    float text_size;
    int left_text_margin;
    int top_text_margin;
    bool mouse_inside;
    bool area_clicked;
    AABB aabb;
    std::function<void()> onClick;

public:
    bool hidden = false;
    GUIItem(olc::vf2d position, const std::string& decal_path, const std::string& text, float size = 1.0, int left_text_margin = 5, int top_text_margin = 5);
    virtual ~GUIItem();

    virtual bool draw();
    virtual void onMouseEntered();
    virtual void onMouseLeft();
    virtual void onMouseClick();

    void hide();
    void show();

    virtual void setOnClick(std::function<void()> onClick);

    const olc::vf2d& getPosition()const;
    void setPosition(const olc::vf2d& position);

    const std::string& getText()const;
    void setText(const std::string& text); 

    float getSize()const;
    void setSize(float size);

    float getTextSize()const;
    void setTextSize(float text_size);

    int getLeftTextMargin()const;
    void setLeftTextMargin(int left_text_margin);

    int getTopTextMargin()const;
    void setTopTextMargin(int top_text_margin);

    const AABB& getAABB()const;

    bool getMouseInside()const;
    bool getAreaClicked()const;

};
#endif