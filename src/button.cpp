#include "button.h"
#include "game.h"

Button::Button(olc::vf2d position, olc::Decal* decal, AABB aabb, const std::string text, int left_text_margin, int top_text_margin, bool resize_button_to_text, olc::Decal* left_part, olc::Decal* middle_part, olc::Decal* right_part)
: GUIItem(position, decal, aabb, text, left_text_margin, top_text_margin), resize_button_to_text(resize_button_to_text), left_part(left_part), middle_part(middle_part), right_part(right_part){
    
}

bool Button::draw(){
    std::cout << "hello" << std::endl;
    if(left_part != NULL && middle_part != NULL && right_part != NULL && resize_button_to_text){
        if(resize_button_to_text){
            game->DrawDecal(position, left_part);
            for(long unsigned int i = 0; i <= text.length() - 1; i++){
                game->DrawDecal({position.x + left_part->sprite->width + i * middle_part->sprite->width, position.y}, middle_part);
            }
            game->DrawDecal({position.x + middle_part->sprite->width * (text.length()) + left_part->sprite->width, position.y}, right_part);
            //game->DrawDecal({position.x + left_part->sprite->width, position.y}, middle_part, {(float)1 * text.length(), 1.0});
        }

    }
    GUIItem::draw();
    return true;

}