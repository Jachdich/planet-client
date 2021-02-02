#include "toggle_button.h"

ToggleButton::ToggleButton(olc::vf2d position, olc::Decal* decal, AABB aabb, const std::string text, bool& toggle_value)
: GUIItem(position, decal, aabb, text), toggle_value(toggle_value){}

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