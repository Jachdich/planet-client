#include "main_menu.h"
#include "game.h"
#include "toggle_button.h"
#include "button.h"

MainMenu::MainMenu() : Scene("textures/menu/background.png"){
    olc::Sprite* spr = new olc::Sprite("textures/buttons/default/left.png");
    olc::Decal* dec = new olc::Decal(spr);
    olc::Sprite* spr2 = new olc::Sprite("textures/buttons/default/middle.png");
    std::ignore = spr2;
    olc::Decal* dec2 = new olc::Decal(spr2);
    std::ignore = dec2;
    olc::Sprite* spr3 = new olc::Sprite("textures/buttons/default/right.png");
    std::ignore = spr3;
    olc::Decal* dec3 = new olc::Decal(spr3);
    std::ignore = dec3;
    Button* item = new Button({0,0}, NULL, AABB(0, 0, 100, 100), "planet name", 10, 5, true, dec, dec2, dec3);
    //GUIItem* item = new GUIItem({0, 0}, dec, AABB(0,0,100,100), "hello");
    items.push_back(item);
}