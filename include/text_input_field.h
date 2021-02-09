#ifndef __TEXT_INPUT_FIELD_H
#define __TEXT_INPUT_FIELD_H

#include "button.h"

class TextInputField : public Button{

private:
    bool focus = false;
    std::string text = "";
    bool enter_pressed = false;
    bool resize = false;
    int default_size;

public:
    TextInputField(const std::string name, olc::vf2d position, const std::string decal_path, bool resize = false, int default_size = 5, float size = 1.0, int alternative_size = 5, int left_text_margin = 10, int top_text_margin = 5, const std::string left_part_path = "textures/buttons/default/left.png", const std::string middle_part_path = "textures/buttons/default/middle.png", const std::string right_part_path = "textures/buttons/default/right.png");

    void onMouseClick() override;
    bool draw() override;

    std::string getText();
    bool getEnterPressed();
};

#endif