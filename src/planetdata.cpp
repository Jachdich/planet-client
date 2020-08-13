#include "planetdata.h"
#include "planetsurface.h"

PlanetData::PlanetData() {}
PlanetData::PlanetData(PlanetSurface * surface) {
	this->surface = surface;
}