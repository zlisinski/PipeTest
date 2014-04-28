#pragma once
#include "SDL.h"
#include "Box2D/Box2D.h"

class CWall
{
public:
	CWall(Uint32 x, Uint32 y, Uint32 width, Uint32 height);
	virtual ~CWall();

	void draw(SDL_Surface *surface);

private:
	Uint32 x;
	Uint32 y;
	Uint32 width;
	Uint32 height;
	b2Body *body;
};

