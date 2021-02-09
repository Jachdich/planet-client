#include "gui_item.h"
#include "game.h"
#include "client.h"

GUIItem::GUIItem(const std::string name, olc::vf2d position, const std::string decal_path, std::string text, float size, int left_text_margin, int top_text_margin) : name(name), position(position), text(text), game(app), size(size), left_text_margin(left_text_margin), top_text_margin(top_text_margin){
    if(!decal_path.empty()){
        sprite = std::make_unique<olc::Sprite>(decal_path);
        GUIItem::decal = std::make_unique<olc::Decal>(sprite.get());
        aabb = AABB(position.x, position.y, GUIItem::decal->sprite->width, GUIItem::decal->sprite->height);
    }
}

GUIItem::~GUIItem(){
}

bool GUIItem::draw(){
    if(!mouse_inside && aabb.isInside(olc::vd2d(game->GetMouseX(), game->GetMouseY()))){
        onMouseEntered();
    }else if(mouse_inside && !aabb.isInside(olc::vd2d(game->GetMouseX(), game->GetMouseY()))){
        onMouseLeft();
    }

    if(mouse_inside && game->GetMouse(0).bPressed){
        area_clicked = true;
        onMouseClick();
    }else{
        area_clicked = false;
    }
    
    if(decal != NULL){
        game->DrawDecal(aabb.pos * olc::vd2d(game->ScreenWidth(), game->ScreenHeight()), decal.get(), {size, size});
    }

    if(!text.empty()){
        game->DrawStringDecal({position.x + left_text_margin * size, position.y + top_text_margin * size}, text, olc::BLACK, {size, size});
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
    return decal.get();
}
