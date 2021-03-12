#include "dropdownButton.h"

DropdownButton::DropdownButton(olc::vf2d position, const std::string& decal_path, const std::string& text, bool resize_button_to_text, float size, int alternative_size, int left_text_margin, int top_text_margin, const std::string& left_part_path, const std::string& middle_part_path, const std::string& right_part_path_closed, const std::string& right_part_path_opened)
: Button(position, decal_path, text, resize_button_to_text, size, alternative_size, left_text_margin, top_text_margin, left_part_path, middle_part_path, right_part_path_closed), right_part_path_closed(right_part_path_closed), right_part_path_opened(right_part_path_opened){
    setOnClick([&](){toggleDropdownItems(); std::cout << "hello world" << std::endl;});
    rightPartOpen = std::make_unique<olc::Sprite>(right_part_path_opened);
    rightPartClosed = std::make_unique<olc::Sprite>(right_part_path_closed);
}

void DropdownButton::addDropdownItem(Button* item){
    item->setTextSize(size);
    item->setSize(size);
    subButtons.push_back(item);
}
void DropdownButton::removeAllDropdownItems(){
    for(Button* item : subButtons){
        delete item;
    }
}

void DropdownButton::setOnClick(std::function<void()> onClick){
    GUIItem::setOnClick(onClick);
}

void DropdownButton::removeDropdownItem(){
    subButtons.pop_back();
}

void DropdownButton::showDropdownItems(){
    rightPart = std::make_unique<olc::Decal>(rightPartOpen.get()); 
}

void DropdownButton::hideDropdownItems(){
    rightPart = std::make_unique<olc::Decal>(rightPartClosed.get()); 
}

void DropdownButton::toggleDropdownItems(){
    if(!dropDownItemsHidden){
        hideDropdownItems();
    }else{
        showDropdownItems();
    }
    dropDownItemsHidden = !dropDownItemsHidden;
}

bool DropdownButton::draw(){
    
    if(!dropDownItemsHidden){

        float offsetY = leftPart->sprite->width;
        for(Button* item : subButtons){
            item->setPosition({position.x + 5 * size, position.y + leftPart->sprite->width + offsetY});
            offsetY += item->getLeftPart()->sprite->height * item->getSize();
            item->draw();
        }
    }
    
    Button::draw();

    return true;
}