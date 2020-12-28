#include "menu.h"
#include "sprites.h"
#include "game.h"

Menu::Menu() {

}

void Menu::draw(Game *e) {
    UIComponent bg = UIComponents["main_menu_background"];
    UIComponent buttons = UIComponents["main_menu_buttons"];
    e->DrawDecal({0, 0}, bg.decal, 1.0f / pixelsPerPixel);
    e->DrawDecal({0, 0}, buttons.decal, 1.0f / pixelsPerPixel);
}