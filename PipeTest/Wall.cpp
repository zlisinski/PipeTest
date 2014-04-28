#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"
#include "main.h"
#include "Wall.h"


CWall::CWall(Uint32 x, Uint32 y, Uint32 width, Uint32 height) : x(x), y(y), width(width), height(height)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter((float)x - (width / 2)), pixelToMeter((float)y - (height / 2)));
	
	body = world->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(pixelToMeter((float)width / 2), pixelToMeter((float)height / 2));
	body->CreateFixture(&box, 0);
}

CWall::~CWall()
{
	world->DestroyBody(body);
}

void CWall::draw(SDL_Surface *surface)
{
	Draw_FillRect(surface, x, y, width, height, SDL_MapRGB(surface->format, 128, 128, 0));
}