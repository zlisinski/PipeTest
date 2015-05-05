#pragma once

#include "SDL.h"
#include "Box2D/Box2D.h"

class CAbstractBody
{
public:
	CAbstractBody();
	CAbstractBody(int x, int y);
	CAbstractBody(int x, int y, Uint32 color);
	CAbstractBody(const CAbstractBody &copy);
	CAbstractBody &operator=(const CAbstractBody &copy);
	virtual ~CAbstractBody();

	virtual void move(Uint32 x, Uint32 y);

	/// Updates local data from Box2d data.
	virtual void update();
	virtual void draw(SDL_Surface *surface, unsigned int frame) const = 0;
	virtual bool testPoint(Uint32 x, Uint32 y);

	int getX() const {return x;}
	int getY() const {return y;}
	Uint32 getColor() const {return color;}

	b2Body *getBody() {return this->body;}

protected:
	int x;
	int y;
	Uint32 color;
	b2Body *body;

	virtual Uint32 randomColor() const;
};

