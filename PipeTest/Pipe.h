#pragma once

#include "abstractbody.h"
#include "Drop.h"

class CPipe : public CAbstractBody
{
public:
	CPipe();
	CPipe(int x, int y, int width, int height);
	CPipe(int x, int y, int width, int height, Uint32 color);
	CPipe(const CPipe &copy);
	CPipe &operator=(const CPipe &copy);
	virtual ~CPipe();
	
	virtual void draw(SDL_Surface *surface, unsigned int frame) const;

	void captureDrop(CDrop &drop, unsigned int frame);
	void captureDrops(std::list<CDrop> &drops, unsigned int frame);

	int getWidth() const {return width;}
	int getHeight() const {return height;}

private:
	int width;
	int height;
	unsigned int frame;
	std::list<CDrop> drops;
	
	virtual b2Body *createBody() const;
};

