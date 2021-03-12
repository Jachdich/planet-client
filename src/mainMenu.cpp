#include "mainMenu.h"
#include "multiplayerMenu.h"
#include "game.h"
#include "button.h"
#include "client.h"
#include "textInputField.h"
#include "dropdownButton.h"

MainMenu::MainMenu() : Scene("textures/backgrounds/background_main.png"){
    Button* item =  new Button({0, 400}, "", "Singleplayer", false, 1.5, 16);
    item->setOnClick([](){});
    Button* item2 = new Button({0,430}, "", "Multiplayer", false, 1.5, 16);
    item2->setOnClick([](){app->changeScene(app->multiPlayerMenu.get());});
    Button* item3 = new Button({0,460}, "", "Options", false, 1.5, 16);
    Button* item4 = new Button({0,490}, "", "Exit game", false, 1.5, 16);
    item4->setOnClick([](){app->exit = true;});
    items.push_back(item);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
}

bool MainMenu::draw(){
    Scene::draw();
    return true;
}
