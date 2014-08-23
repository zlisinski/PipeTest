#pragma once

#include "AbstractBody.h"

class CDrop : public CAbstractBody
{
public:
	CDrop();
	CDrop(int x, int y, int radius, const b2Vec2 &velocity);
	CDrop(int x, int y, int radius, const b2Vec2 &velocity, Uint32 color);
	CDrop(const CDrop &copy);
	CDrop &operator=(const CDrop &copy);
	virtual ~CDrop();

	virtual void draw(SDL_Surface *surface, unsigned int frame) const;

	int getRadius() const {return radius;}
	const b2Vec2 &getVelocity() const {return this->body->GetLinearVelocity();}

private:
	int radius;

	b2Body *createBody(const b2Vec2 &velocity) const;
	virtual Uint32 randomColor() const;
};

