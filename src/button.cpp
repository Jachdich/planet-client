#include "button.h"
#include "game.h"

Button::Button(const std::string name, olc::vf2d position, const std::string decal_path, const std::string text, bool resize_button_to_text, float size, int alternative_size, int left_text_margin, int top_text_margin, const std::string left_part_path, const std::string middle_part_path, const std::string right_part_path)
: GUIItem(name, position, decal_path, text, size, left_text_margin, top_text_margin), resize_button_to_text(resize_button_to_text), alternative_size(alternative_size * size){
    if(decal_path.empty()){
        if(!left_part_path.empty() && !middle_part_path.empty() && !right_part_path.empty()){
        
            left_part_sprite = std::make_unique<olc::Sprite>(left_part_path);
            middle_part_sprite = std::make_unique<olc::Sprite>(middle_part_path);
            right_part_sprite = std::make_unique<olc::Sprite>(right_part_path);
            left_part = std::make_unique<olc::Decal>(left_part_sprite.get());
            middle_part = std::make_unique<olc::Decal>(middle_part_sprite.get());
            right_part = std::make_unique<olc::Decal>(right_part_sprite.get());

            if(resize_button_to_text)
                aabb = AABB(position.x, position.y, left_part->sprite->width + middle_part->sprite->width * text.length() * size + right_part->sprite->width * size, left_part->sprite->height * size);
            else
                aabb = AABB(position.x, position.y, left_part->sprite->width + middle_part->sprite->width * alternative_size * size + right_part->sprite->width * size, left_part->sprite->height * size);
            }
        }
}

bool Button::draw(){
    if(decal == NULL){
        if(left_part != NULL && middle_part != NULL && right_part != NULL){
            if(resize_button_to_text){
                game->DrawDecal(position, left_part.get(), {size, size});
                if(!text.length() - 1 <= 0){
                    for(long unsigned int i = 0; i <= text.length() - 1; i++){
                        game->DrawDecal({position.x + left_part->sprite->width + i * middle_part->sprite->width, position.y}, middle_part.get(), {size, size});
                    }
                }
                game->DrawDecal({position.x + middle_part->sprite->width * (text.length()) + left_part->sprite->width, position.y}, right_part.get(), {size, size});
                //game->DrawDecal({position.x + left_part->sprite->width, position.y}, middle_part, {(float)1 * text.length(), 1.0});
            }else if(!resize_button_to_text && alternative_size > 0){
                game->DrawDecal(position, left_part.get(), {size, size});
                for(int i = 0; i <= alternative_size; i++){
                    game->DrawDecal({position.x + left_part->sprite->width + i * middle_part->sprite->width, position.y}, middle_part.get(), {size, size});
                }
            
                game->DrawDecal({position.x + middle_part->sprite->width * alternative_size + left_part->sprite->width, position.y}, right_part.get(), {size, size});
            }
        }
    }

    GUIItem::draw();
    return true;

}

void Button::setLeftPart(const std::string& left_part_path){
    this->left_part_sprite = std::make_unique<olc::Sprite>(left_part_path);
    this->left_part = std::make_unique<olc::Decal>(left_part_sprite.get());
}

void Button::setMiddlePart(const std::string& middle_part_path){
    this->middle_part_sprite = std::make_unique<olc::Sprite>(middle_part_path);
    this->middle_part = std::make_unique<olc::Decal>(middle_part_sprite.get());
}

void Button::setRightPart(const std::string& right_part_path){
    this->right_part_sprite = std::make_unique<olc::Sprite>(right_part_path);
    this->right_part = std::make_unique<olc::Decal>(right_part_sprite.get());
}