#pragma once

#include <list>

#include "abstractbody.h"

class CDrop;

class CPipe : public CAbstractBody
{
public:
	enum ESide {Top, Right, Bottom, Left};

	CPipe();
	CPipe(int x, int y, int width, int height, ESide captureSide, ESide emitSide);
	CPipe(int x, int y, int width, int height, ESide captureSide, ESide emitSide, Uint32 color);
	CPipe(const CPipe &copy);
	CPipe &operator=(const CPipe &copy);
	virtual ~CPipe();
	
	virtual void draw(SDL_Surface *surface, unsigned int frame) const;

	bool allowCollision(CDrop &drop);

	bool captureDrop(CDrop &drop, unsigned int frame);
	std::list<CDrop *> emitDrops(unsigned int frame);

	int getWidth() const {return width;}
	int getHeight() const {return height;}
	int getMinX() const {return this->x;}
	int getMaxX() const {return this->x + this->width;}
	int getMinY() const {return this->y;}
	int getMaxY() const {return this->y + this->height;}

	void setCaptureSide(ESide side) {this->captureSide = side;}
	void setEmitSide(ESide side) {this->emitSide = side;}

private:
	int width;
	int height;
	unsigned int frame;
	std::list<CDrop *> drops;
	ESide captureSide;
	ESide emitSide;
	
	b2Body *createBody() const;
};

