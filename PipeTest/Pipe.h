#pragma once

#include "abstractbody.h"

class CPipe : public CAbstractBody
{
public:
	CPipe();
	CPipe(int x, int y, int width, int height);
	CPipe(int x, int y, int width, int height, Uint32 color);
	CPipe(const CPipe &copy);
	CPipe &operator=(const CPipe &copy);
	virtual ~CPipe();
	
	virtual void draw(SDL_Surface *surface) const;

	int getWidth() const {return width;}
	int getHeight() const {return height;}

private:
	int width;
	int height;
	
	virtual b2Body *createBody() const;
};

