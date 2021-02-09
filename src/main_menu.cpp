#include "main_menu.h"
#include "multiplayer_menu.h"
#include "game.h"
#include "button.h"
#include "client.h"
#include "text_input_field.h"

MainMenu::MainMenu() : Scene("textures/backgrounds/background_main.png"){
    Button* item =  new Button("singleplayer", {0, 400}, "", "Singleplayer", false, 1.5, 16);
    Button* item2 = new Button("multiplayer", {0,430}, "", "Multiplayer", false, 1.5, 16);
    Button* item3 = new Button("options", {0,460}, "", "Options", false, 1.5, 16);
    Button* item4 = new Button("exit_game", {0,490}, "", "Exit game", false, 1.5, 16);
    //TextInputField* field = new TextInputField("input_field", {0, 0}, "", true, 5, 1.0f, 10);
    items.push_back(item);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
   // items.push_back(field);
}

void MainMenu::onGUIItemClicked(const std::string& name){

    if(name == "multiplayer")
        app->changeScene(app->multiPlayerMenu.get());
    if(name == "exit_game")
        app->exit = true;

}

bool MainMenu::draw(){
    Scene::draw();
    return true;
}

void MainMenu::checkForInput(){
    Scene::checkForInput();
}