#ifndef __TOGGLE_BUTTON_H
#define __TOGGLE_BUTTON_H

#include "gui_item.h"

class ToggleButton : public GUIItem{

private:
    bool& toggle_value;

public:
    ToggleButton(olc::vf2d position, const std::string decal_path, const std::string text, bool& toggle_value, float size = 1.0);

    bool draw();
    void onMouseClick();

    void setToggleValue(bool& toggle_value);
    bool getToggleValue();
};

#endif