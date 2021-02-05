#include "main_menu.h"
#include "game.h"
#include "toggle_button.h"
#include "button.h"

MainMenu::MainMenu() : Scene("textures/menu/background.png"){
    Button* item = new Button({0,400}, "", "Singleplayer", false, 1.5, 16);
    Button* item2 = new Button({0,430}, "", "Multiplayer", false, 1.5, 16);
    Button* item3 = new Button({0,460}, "", "Options", false, 1.5, 16);
    Button* item4 = new Button({0,490}, "", "Exit game", false, 1.5, 16);
    items.push_back(item);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
}