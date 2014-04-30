#include <cmath>
#include "SDL.h"
#include "SDL_draw.h"
#include "main.h"
#include "Drop.h"

CDrop::CDrop()
{
	this->radius = 5 + (rand() % 10);
	this->x = SCREEN_WIDTH - 50 + (rand() % 20);
	this->y = 100 + (rand() % 20);
	Uint32 r = rand() % 128;
	Uint32 g = 128 + (rand() % 128);
	Uint32 b = rand() % 128;
	this->color = SDL_MapRGB(screen->format, r, g, b);
	this->body = this->createBody();
}

CDrop::CDrop(Uint32 radius, Uint32 x, Uint32 y, Uint32 color) : radius(radius), x(x), y(y), color(color)
{
	this->body = this->createBody();
}

CDrop::CDrop(const CDrop &copy) : radius(copy.radius), x(copy.x), y(copy.y), color(copy.color)
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
	world->DestroyBody(this->body);
}

void CDrop::move(Uint32 x, Uint32 y)
{
	this->x = x;
	this->y = y;
}

void CDrop::draw(SDL_Surface *surface) const
{
	Draw_FillCircle(surface, this->x, this->y, this->radius, this->color);
}

void CDrop::update()
{
	b2Vec2 pos = this->body->GetPosition();
	this->x = (Uint32)ceil(meterToPixel(pos.x));
	this->y = (Uint32)ceil(meterToPixel(pos.y));
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