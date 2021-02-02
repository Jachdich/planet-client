#ifndef __BUTTON_H
#define __BUTTON_H

#include "gui_item.h"

class Button : public GUIItem{

protected:
    bool resize_button_to_text;
    olc::Decal* left_part;
    olc::Decal* middle_part;
    olc::Decal* right_part;
public:
    Button(olc::vf2d position, olc::Decal* decal, AABB aabb, const std::string text, int left_text_margin = 10, int top_text_margin = 5, bool resize_button_to_text = false, olc::Decal* left_part = NULL, olc::Decal* middle_part = NULL, olc::Decal* right_part = NULL);

    bool draw();

};

#endif