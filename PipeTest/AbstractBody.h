#pragma once

#include "SDL.h"
#include "Box2D/Box2D.h"

class CAbstractBody
{
public:
	CAbstractBody();
	CAbstractBody(int x, int y, Uint32 color);
	CAbstractBody(const CAbstractBody &copy);
	CAbstractBody &operator=(const CAbstractBody &copy);
	virtual ~CAbstractBody();

	virtual void move(Uint32 x, Uint32 y);
	virtual void update();
	virtual void draw(SDL_Surface *surface) const = 0;

	int getX() const {return x;}
	int getY() const {return y;}
	Uint32 getColor() const {return color;}

protected:
	int x;
	int y;
	Uint32 color;
	b2Body *body;

	virtual b2Body *createBody() const = 0;
};

