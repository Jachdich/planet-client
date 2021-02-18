#ifndef __BUTTON_H
#define __BUTTON_H

#include "guiItem.h"

class Button : public GUIItem{

private:
    std::unique_ptr<olc::Sprite> leftPartSprite;
    std::unique_ptr<olc::Sprite> middlePartSprite;
    std::unique_ptr<olc::Sprite> rightPartSprite;
    std::unique_ptr<olc::Sprite> decalSprite;

protected:
    bool resize_button_to_text;
    int alternative_size;
    std::unique_ptr<olc::Decal> leftPart;
    std::unique_ptr<olc::Decal> middlePart;
    std::unique_ptr<olc::Decal> rightPart;
    std::unique_ptr<olc::Decal> decal;
public:
    Button(olc::vf2d position, const std::string& decal_path, const std::string& text, bool resize_button_to_text = false, float size = 1.0, int alternative_size = 5, int left_text_margin = 10, int top_text_margin = 5, const std::string& left_part_path = "textures/buttons/default/standard/left.png", const std::string& middle_part_path = "textures/buttons/default/standard/middle.png", const std::string& right_part_path = "textures/buttons/default/standard/right.png");

    void setLeftPart(const std::string& left_part_path);
    olc::Decal* getLeftPart();
    void setMiddlePart(const std::string& middle_part_path);
    olc::Decal* getMiddlePart();
    void setRightPart(const std::string & right_part_path);
    olc::Decal* getRightPart();
    bool draw() override;

};
#endif