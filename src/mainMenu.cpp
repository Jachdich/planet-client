#include "mainMenu.h"
#include "multiplayerMenu.h"
#include "game.h"
#include "button.h"
#include "client.h"
#include "textInputField.h"

using namespace GUI;

MainMenu::MainMenu() : Scene("textures/backgrounds/background_main.png"){
    Button* item =  new Button("singleplayer", {0, 400}, "", "Singleplayer", false, 1.5, 16);
    item->setOnClick([](){std::cout << "hello" << std::endl;});
    Button* item2 = new Button("multiplayer", {0,430}, "", "Multiplayer", false, 1.5, 16);
    item2->setOnClick([](){app->changeScene(app->multiPlayerMenu.get());});
    Button* item3 = new Button("options", {0,460}, "", "Options", false, 1.5, 16);
    Button* item4 = new Button("exit_game", {0,490}, "", "Exit game", false, 1.5, 16);
    item3->setOnClick([](){app->exit = true;});
    //TextInputField* field = new TextInputField("input_field", {0, 0}, "", true, 5, 1.0f, 10);
    items.push_back(item);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
   // items.push_back(field);
}

bool MainMenu::draw(){
    Scene::draw();
    return true;
}
