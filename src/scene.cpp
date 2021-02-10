#include "scene.h"
#include "game.h"
#include "client.h"
#include "sprites.h"

Scene::Scene(const std::string background_path){
    background_sprite = std::make_unique<olc::Sprite>(background_path);
    background = std::make_unique<olc::Decal>(background_sprite.get());
}

Scene::~Scene(){
    for(GUIItem* item : items){
        delete item;
    }
}

bool Scene::draw(){
    
    if(background != NULL){
        app->DrawDecal({0, 0}, background.get());
    }

    for(GUIItem* item : items){
        if(!item->draw()){
            return false;
        }
    }

    checkForInput();
    return true;
}

void Scene::checkForInput(){
    for(GUIItem* i : items){
        if(i->getAreaClicked()){
            onGUIItemClicked(i->getName());
        }
    }
}

void Scene::onGUIItemClicked(const std::string& name){
}