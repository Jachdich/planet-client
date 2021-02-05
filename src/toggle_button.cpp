#include "toggle_button.h"

ToggleButton::ToggleButton(olc::vf2d position, const std::string decal_path, const std::string text, bool& toggle_value, float size)
: GUIItem(position, decal_path, text, size), toggle_value(toggle_value){}

bool ToggleButton::draw(){
    GUIItem::draw();
    return true;
}

void ToggleButton::onMouseClick(){
    GUIItem::onMouseClick();

    if(!toggle_value)toggle_value = true;
    else{toggle_value = false;}
}

void ToggleButton::setToggleValue(bool& toggle_value){
    this->toggle_value = toggle_value;
}

bool ToggleButton::getToggleValue(){
    return this->toggle_value;
}