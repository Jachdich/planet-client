#ifndef __DROPDOWNBUTTON_H
#define __DROPDOWNBUTTON_H

#include "button.h"

class DropdownButton : public Button{

protected:
    bool dropDownItemsHidden = true;
    std::vector<Button*> subButtons;
    const std::string right_part_path_closed;
    const std::string right_part_path_opened;
    std::unique_ptr<olc::Sprite> rightPartClosed;
    std::unique_ptr<olc::Sprite> rightPartOpen;

private:
    void setOnClick(std::function<void()> onClick)override;

public:
    DropdownButton(olc::vf2d position, const std::string& decal_path, const std::string& text, bool resize_button_to_text = false, float size = 1.0, int alternative_size = 5, int left_text_margin = 10, int top_text_margin = 5, const std::string& left_part_path = "textures/buttons/default/standard/left.png", const std::string& middle_part_path = "textures/buttons/default/standard/middle.png", const std::string& right_part_path_closed = "textures/buttons/default/dropdown/right_closed.png", const std::string& right_part_path_opened = "textures/buttons/default/dropdown/right_open.png");

    void addDropdownItem(Button* item);
    void removeDropdownItem();
    void showDropdownItems();
    void hideDropdownItems();
    void toggleDropdownItems();
    bool draw() override;

};

#endif