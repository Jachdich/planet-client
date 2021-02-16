#include "guiItem.h"
#include "game.h"
#include "client.h"
GUIItem::GUIItem(const std::string name, olc::vf2d position, const std::string& decal_path, const std::string& text, float size, int left_text_margin, int top_text_margin) 
: name(name), position(position), text(text), game(app), size(size), text_size(size), left_text_margin(left_text_margin), top_text_margin(top_text_margin){
    if(!decal_path.empty()){
        sprite = std::make_unique<olc::Sprite>(decal_path);
        GUIItem::decal = std::make_unique<olc::Decal>(sprite.get());
        aabb = AABB(position.x, position.y, GUIItem::decal->sprite->width * size, GUIItem::decal->sprite->height * size);
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
        game->DrawDecal(position, decal.get(), {size, size});
    }

    if(!text.empty()){
        game->DrawStringDecal({position.x + left_text_margin * size, position.y + top_text_margin * size}, text, olc::BLACK, {text_size, text_size});
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

const std::string& GUIItem::getName()const{
    return name;
}

const olc::vf2d& GUIItem::getPosition()const{
    return position;
}
void GUIItem::setPosition(const olc::vf2d& position){
    this->position = position;
}

const std::string& GUIItem::getText()const{
    return text;
}
void GUIItem::setText(const std::string& text){
    this->text = text;
}

float GUIItem::getSize() const{
    return size;
}
void GUIItem::setSize(float size){
    this->size = size;
}

float GUIItem::getTextSize()const{
    return text_size;
}
void GUIItem::setTextSize(float text_size){
    this->text_size = text_size;
}

int GUIItem::getLeftTextMargin()const{
    return left_text_margin;
}
void GUIItem::setLeftTextMargin(int left_text_margin){
    this->left_text_margin = left_text_margin;
}

int GUIItem::getTopTextMargin()const{
    return top_text_margin;
}
void GUIItem::setTopTextMargin(int top_text_margin){
    this->top_text_margin = top_text_margin;
}

const AABB& GUIItem::getAABB()const{
    return aabb;
}

bool GUIItem::getMouseInside()const{
    return mouse_inside;
}
bool GUIItem::getAreaClicked()const{
    return area_clicked;
}

//olc::Decal* GUIItem::getDecal()const{
 //   return decal.get();
//}
//void GUIItem::setDecal(olc::Decal* decal){
//    this->decal = std::make_unique<olc::Decal>(decal);
//}