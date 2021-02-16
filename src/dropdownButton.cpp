#include "dropdownButton.h"

DropdownButton::DropdownButton(const std::string name, olc::vf2d position, const std::string decal_path, const std::string text, bool resize_button_to_text, float size, int alternative_size, int left_text_margin, int top_text_margin, const std::string left_part_path, const std::string middle_part_path, const std::string right_part_path_closed, const std::string right_part_path_opened)
: Button(name, position, decal_path, text, resize_button_to_text, size, alternative_size, left_text_margin, top_text_margin, left_part_path, middle_part_path, right_part_path_closed), right_part_path_closed(right_part_path_closed), right_part_path_opened(right_part_path_opened){}

void DropdownButton::addDropdownItem(GUIItem* item){
    subButtons.push_back(item);
}

void DropdownButton::removeDropdownItem(){
    subButtons.pop_back();
}

bool DropdownButton::draw(){
    Button::draw();
    
    return true;
}