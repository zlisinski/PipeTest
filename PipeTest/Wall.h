#pragma once
#include "SDL.h"
#include "Box2D/Box2D.h"

class CWall
{
public:
	CWall(int x, int y, int width, int height, Uint32 color);
	CWall(const CWall &copy);
	CWall &operator=(const CWall &copy);
	virtual ~CWall();

	void draw(SDL_Surface *surface);

private:
	int x;
	int y;
	int width;
	int height;
	Uint32 color;
	b2Body *body;

	b2Body *createBody() const;
};

