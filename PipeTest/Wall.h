#pragma once

#include "AbstractBody.h"

class CWall : public CAbstractBody
{
public:
	CWall();
	CWall(int x, int y, int width, int height, Uint32 color);
	CWall(const CWall &copy);
	CWall &operator=(const CWall &copy);
	virtual ~CWall();

	virtual void draw(SDL_Surface *surface) const;

	int getWidth() const {return width;}
	int getHeight() const {return height;}

private:
	int width;
	int height;
	
	virtual b2Body *createBody() const;
};

