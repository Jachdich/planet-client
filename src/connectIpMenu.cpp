#include "connectIpMenu.h"
#include "textInputField.h"
#include "client.h"
#include "mainMenu.h"

ConnectToIpMenu::ConnectToIpMenu() : Scene(){

    ip = new TextInputField("ip", {0, 380}, "textures/buttons/multiplayer/connecttoip.png", false, 16, 1.5, 5, 18, 38);
    ip->setTextSize(1.5);
    ip->setOnEnter([&](){
        app->address = ip->getText();
        if(!app->connectToServer())return;
        app->changeScene(app->galaxy.get());
    });
    Button* item =  new Button("back", {0, 490}, "", "Back", false, 1.5, 16);
    item->setOnClick([](){app->changeScene(app->multiPlayerMenu.get());});
    items.push_back(item);
    items.push_back(ip);

}

bool ConnectToIpMenu::draw(){
    Scene::draw();
    return true;
}
