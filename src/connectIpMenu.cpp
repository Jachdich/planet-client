#include "connectIpMenu.h"
#include "textInputField.h"
#include "client.h"
#include "mainMenu.h"

using namespace GUI;

ConnectToIpMenu::ConnectToIpMenu() : Scene(){

    ip = new TextInputField("ip", {0, 380}, "textures/buttons/multiplayer/connecttoip.png", false, 16, 1.5, 5, 18, 38);
    ip->setTextSize(1.5);
    Button* item =  new Button("back", {0, 490}, "", "Back", false, 1.5, 16);
    items.push_back(item);
    items.push_back(ip);

}

bool ConnectToIpMenu::draw(){
    Scene::draw();
    if(ip->getEnterPressed()){
        app->address = ip->getText();
        if(!app->connectToServer())return true;
        app->changeScene(app->galaxy.get());
        //app->galaxyView = true;
    }
    return true;
}

void ConnectToIpMenu::onGUIItemClicked(const std::string& name){

    if(name == "back"){
        app->changeScene(app->multiPlayerMenu.get());
    }

}

void ConnectToIpMenu::onEnterPressed(const std::string& name, const std::string& text){
    std::cout << text + "coming from: " + name << std::endl;
}