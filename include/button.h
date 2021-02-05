#ifndef __BUTTON_H
#define __BUTTON_H

#include "gui_item.h"

class Button : public GUIItem{

private:
    std::unique_ptr<olc::Sprite> left_part_sprite;
    std::unique_ptr<olc::Sprite> middle_part_sprite;
    std::unique_ptr<olc::Sprite> right_part_sprite;

protected:
    bool resize_button_to_text;
    int alternative_size;
    std::unique_ptr<olc::Decal> left_part;
    std::unique_ptr<olc::Decal> middle_part;
    std::unique_ptr<olc::Decal> right_part;
public:
    Button(olc::vf2d position, const std::string decal_path, const std::string text, bool resize_button_to_text = false, float size = 1.0, int alternative_size = 5, int left_text_margin = 10, int top_text_margin = 5, const std::string left_part_path = "textures/buttons/default/left.png", const std::string middle_part_path = "textures/buttons/default/middle.png", const std::string right_part_path = "textures/buttons/default/right.png");

    bool draw();

};

#endif