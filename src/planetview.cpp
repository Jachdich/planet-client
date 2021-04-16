#include "planetview.h"
#include "client.h"
#include "planetdata.h"

PlanetView::PlanetView() : Scene(""){
    curr_ddb = new DropdownButton({0, 0}, "", "Demolition", true);  
    items.push_back(curr_ddb);
	curr_ddb->hidden = true;
}

bool PlanetView::draw(float fElapsedTime){
    
    if (app->selectedPlanet->surface->generated) {
    	    app->selectedPlanet->surface->data->updateTimers(fElapsedTime);
			app->selectedPlanet->surface->mouseOver(app->GetMouseX(), app->GetMouseY(), app->GetMouse(0).bPressed, app->GetMouse(0).bHeld, app->trx, curr_ddb);
    		app->selectedPlanet->drawSurface(app, app->trx);
    }

    if(app->GetKey(olc::Key::ESCAPE).bPressed){
        app->changeScene(app->starViewScene.get());
    }
    Scene::draw();

    return true;

}
