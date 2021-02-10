#include "connect_ip_menu.h"
#include "text_input_field.h"
#include "client.h"
#include "main_menu.h"

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
        app->connectToServer();
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