#include "planetview.h"
#include "client.h"
#include "planetdata.h"

PlanetView::PlanetView() : Scene(""){

}

bool PlanetView::draw(){
    Scene::draw();
    
    if (app->selectedPlanet->surface->generated) {
    	    app->selectedPlanet->surface->data->updateTimers(fElapsedTime);
			app->selectedPlanet->surface->mouseOver(app->GetMouseX(), app->GetMouseY(), app->GetMouse(0).bPressed, app->GetMouse(0).bHeld, app->trx);
    		app->selectedPlanet->drawSurface(app, app->trx);
    }

    if(app->GetKey(olc::Key::ESCAPE).bPressed){
        app->changeScene(app->starViewScene.get());
    }

    return true;

}