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
	int getMinX() const {return this->x - this->radius;}
	int getMaxX() const {return this->x + this->radius;}
	int getMinY() const {return this->y - this->radius;}
	int getMaxY() const {return this->y + this->radius;}
	const b2Vec2 &getVelocity() const {return this->body->GetLinearVelocity();}

	bool getCaptured() {return this->captured;}
	void setCaptured(bool val) {this->captured = val;}

private:
	int radius;
	bool captured;

	b2Body *createBody(const b2Vec2 &velocity) const;
	virtual Uint32 randomColor() const;
};

