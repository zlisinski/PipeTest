#pragma once

#include "SDL.h"
#include "Box2D/Box2D.h"

#include "AbstractBody.h"

class CDrop : public CAbstractBody
{
public:
	CDrop();
	CDrop(int x, int y, int radius, Uint32 color);
	CDrop(const CDrop &copy);
	CDrop &operator=(const CDrop &copy);
	virtual ~CDrop();

	virtual void draw(SDL_Surface *surface) const;

	int getRadius() const {return radius;}

private:
	int radius;

	virtual b2Body *createBody() const;
};

