#pragma once

#include "SDL.h"
#include "Box2D/Box2D.h"

class CDrop
{
public:
	CDrop();
	CDrop(int radius, int x, int y, Uint32 color);
	CDrop(const CDrop &copy);
	CDrop &operator=(const CDrop &copy);
	virtual ~CDrop();

	void move(Uint32 x, Uint32 y);
	void draw(SDL_Surface *surface) const;
	void update();

	int getRadius() const {return radius;}
	int getX() const {return x;}
	int getY() const {return y;}
	Uint32 getColor() const {return color;}

private:
	int radius;
	int x;
	int y;
	Uint32 color;
	b2Body *body;

	b2Body *createBody() const;
};

