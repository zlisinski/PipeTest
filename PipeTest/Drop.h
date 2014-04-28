#pragma once

#include "SDL.h"
#include "Box2D/Box2D.h"

class CDrop
{
public:
	CDrop();
	CDrop(Uint32 radius, Uint32 x, Uint32 y, Uint32 color);
	CDrop(const CDrop &copy);
	CDrop &operator=(const CDrop &copy);
	virtual ~CDrop();

	void move(Uint32 x, Uint32 y);
	void draw(SDL_Surface *surface) const;
	void update();

	Uint32 getRadius() const {return radius;}
	Uint32 getX() const {return x;}
	Uint32 getY() const {return y;}
	Uint32 getColor() const {return color;}

private:
	Uint32 radius;
	Uint32 x;
	Uint32 y;
	Uint32 color;
	b2Body *body;

	b2Body *createBody() const;
};

