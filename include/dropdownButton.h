#ifndef __DROPDOWNBUTTON_H
#define __DROPDOWNBUTTON_H

#include "button.h"

class DropdownButton : public Button{

protected:
    std::vector<GUIItem*> subButtons;
    const std::string& right_part_path_closed;
    const std::string& right_part_path_opened;

public:
    DropdownButton(const std::string name, olc::vf2d position, const std::string decal_path, const std::string text, bool resize_button_to_text = false, float size = 1.0, int alternative_size = 5, int left_text_margin = 10, int top_text_margin = 5, const std::string left_part_path = "textures/buttons/default/left.png", const std::string middle_part_path = "textures/buttons/default/middle.png", const std::string right_part_path_closed = "textures/buttons/default/right.png", const std::string right_part_path_opened = "textures/buttons/default/right.png");

    void addDropdownItem(GUIItem* item);
    void removeDropdownItem();
    bool draw() override;

};

#endif