#include "gui_item.h"
#include "game.h"
#include "client.h"

GUIItem::GUIItem(olc::vf2d position, olc::Decal* decal, AABB aabb, const std::string text, int left_text_margin, int top_text_margin) : position(position), decal(decal), aabb(aabb), text(text), game(app), left_text_margin(left_text_margin), top_text_margin(top_text_margin){

}

bool GUIItem::draw(){
    if(!mouse_inside && aabb.isInside(olc::vd2d(game->GetMouseX(), game->GetMouseY()))){
        onMouseEntered();
    }else if(mouse_inside && !aabb.isInside(olc::vd2d(game->GetMouseX(), game->GetMouseY()))){
        onMouseLeft();
    }

    if(mouse_inside && game->GetMouse(0).bPressed){
        onMouseClick();
    }
    
    if(decal != NULL){
        game->DrawDecal(aabb.pos * olc::vd2d(game->ScreenWidth(), game->ScreenHeight()), decal);
    }

    if(!text.empty()){
        game->DrawStringDecal({(float)aabb.pos.x * game->ScreenWidth() + left_text_margin, (float)aabb.pos.y * game->ScreenHeight() + top_text_margin}, text, olc::BLACK);
    }
    return true;
}

void GUIItem::onMouseEntered(){
    this->mouse_inside = true;
}

void GUIItem::onMouseClick(){
}

void GUIItem::onMouseLeft(){
    this->mouse_inside = false;
}

olc::Decal* GUIItem::getDecal(){
    return decal;
}
