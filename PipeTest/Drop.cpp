#include <cmath>
#include "SDL.h"
#include "SDL_draw.h"
#include "main.h"
#include "Drop.h"

CDrop::CDrop() :
	CAbstractBody()
{
	this->radius = 5 + (rand() % 10);
	this->x = SCREEN_WIDTH - 50 + (rand() % 20);
	this->y = 668 + (rand() % 20);
	Uint32 r = rand() % 128;
	Uint32 g = 128 + (rand() % 128);
	Uint32 b = rand() % 128;
	this->color = SDL_MapRGB(screen->format, r, g, b);

	this->body = this->createBody();
}

CDrop::CDrop(int x, int y, int radius, Uint32 color) :
	CAbstractBody(x, y, color),
	radius(radius)
{
	this->body = this->createBody();
}

CDrop::CDrop(const CDrop &copy) :
	CAbstractBody(copy.x, copy.y, copy.color),
	radius(copy.radius)
{
	this->body = copy.createBody();
}

CDrop &CDrop::operator=(const CDrop &copy)
{
	if (this != &copy) {
		this->radius = copy.radius;
		this->x = copy.x;
		this->y = copy.y;
		this->color = copy.color;
		this->body = copy.createBody();
	}

	return *this;
}

CDrop::~CDrop()
{
}

void CDrop::draw(SDL_Surface *surface) const
{
	int newY = flipYAxis(this->y);
	Draw_FillCircle(surface, this->x, newY, this->radius, this->color);
}

b2Body *CDrop::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(pixelToMeter((float)this->x), pixelToMeter((float)this->y));
	bodyDef.linearVelocity.Set(-4.0f, 0.0f);
	
	newBody = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = pixelToMeter((float)this->radius);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.25f;
	fixtureDef.filter.groupIndex = -1;

	newBody->CreateFixture(&fixtureDef);

	return newBody;
}