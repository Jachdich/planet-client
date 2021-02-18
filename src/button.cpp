#include "button.h"
#include "game.h"

Button::Button(olc::vf2d position, const std::string& decal_path, const std::string& text, bool resize_button_to_text, float size, int alternative_size, int left_text_margin, int top_text_margin, const std::string& left_part_path, const std::string& middle_part_path, const std::string& right_part_path)
: GUIItem(position, decal_path, text, size, left_text_margin, top_text_margin), resize_button_to_text(resize_button_to_text), alternative_size(alternative_size * size){
    if(decal_path.empty()){
        if(!left_part_path.empty() && !middle_part_path.empty() && !right_part_path.empty()){
        
            leftPartSprite = std::make_unique<olc::Sprite>(left_part_path);
            middlePartSprite = std::make_unique<olc::Sprite>(middle_part_path);
            rightPartSprite = std::make_unique<olc::Sprite>(right_part_path);
            leftPart = std::make_unique<olc::Decal>(leftPartSprite.get());
            middlePart = std::make_unique<olc::Decal>(middlePartSprite.get());
            rightPart = std::make_unique<olc::Decal>(rightPartSprite.get());

            if(resize_button_to_text)
                aabb = AABB(position.x, position.y, leftPart->sprite->width + middlePart->sprite->width * text.length() * size + rightPart->sprite->width * size, leftPart->sprite->height * size);
            else
                aabb = AABB(position.x, position.y, leftPart->sprite->width + middlePart->sprite->width * alternative_size * size + rightPart->sprite->width * size, leftPart->sprite->height * size);
            }
        }
}

bool Button::draw(){
    if(!hidden){
    GUIItem::draw();
    if(decal == NULL){
        if(leftPart != NULL && middlePart != NULL && rightPart != NULL){
            if(resize_button_to_text){
                game->DrawDecal(position, leftPart.get(), {size, size});
                if(!text.length() - 1 <= 0){
                    for(long unsigned int i = 0; i <= text.length() - 1; i++){
                        game->DrawDecal({position.x + leftPart->sprite->width + i * middlePart->sprite->width, position.y}, middlePart.get(), {size, size});
                    }
                }
                game->DrawDecal({position.x + middlePart->sprite->width * (text.length()) + leftPart->sprite->width, position.y}, rightPart.get(), {size, size});
                //game->DrawDecal({position.x + left_part->sprite->width, position.y}, middle_part, {(float)1 * text.length(), 1.0});
            }else if(!resize_button_to_text && alternative_size > 0){
                game->DrawDecal(position, leftPart.get(), {size, size});
                for(int i = 0; i <= alternative_size; i++){
                    game->DrawDecal({position.x + leftPart->sprite->width + i * middlePart->sprite->width, position.y}, middlePart.get(), {size, size});
                }

            
                game->DrawDecal({position.x + middlePart->sprite->width * alternative_size + leftPart->sprite->width, position.y}, rightPart.get(), {size, size});
                
            }
        }
        game->DrawStringDecal({position.x + left_text_margin * size, position.y + top_text_margin * size}, text, olc::BLACK, {text_size, text_size});
    }

    }

    return true;

}

void Button::setLeftPart(const std::string& left_part_path){
    this->leftPartSprite = std::make_unique<olc::Sprite>(left_part_path);
    this->leftPart = std::make_unique<olc::Decal>(leftPartSprite.get());
}

olc::Decal* Button::getLeftPart(){
    return leftPart.get();
}

void Button::setMiddlePart(const std::string& middle_part_path){
    this->middlePartSprite = std::make_unique<olc::Sprite>(middle_part_path);
    this->middlePart = std::make_unique<olc::Decal>(middlePartSprite.get());
}

olc::Decal* Button::getMiddlePart(){
    return middlePart.get();
}

void Button::setRightPart(const std::string& right_part_path){
    this->rightPartSprite = std::make_unique<olc::Sprite>(right_part_path);
    this->rightPart = std::make_unique<olc::Decal>(rightPartSprite.get());
}

olc::Decal* Button::getRightPart(){
    return rightPart.get();
}