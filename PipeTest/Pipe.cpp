#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Pipe.h"

CPipe::CPipe() :
	CAbstractBody(),
	width(100),
	height(100)
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height) :
	CAbstractBody(x, y),
	width(width),
	height(height)
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height, Uint32 color) :
	CAbstractBody(x, y, color),
	width(width),
	height(height)
{
	this->body = this->createBody();
}

CPipe::CPipe(const CPipe &copy) :
	CAbstractBody(copy),
	width(copy.width),
	height(copy.height)
{
	this->body = copy.createBody();
}

CPipe &CPipe::operator=(const CPipe &copy)
{
	if (this != &copy) {
		this->x = copy.x;
		this->y = copy.y;
		this->width = copy.width;
		this->height = copy.height;
		this->color = copy.color;
		this->body = copy.createBody();
	}

	return *this;
}

CPipe::~CPipe()
{
}

void CPipe::draw(SDL_Surface *surface) const
{
	int newY = flipYAxis(this->y);
	Draw_FillRect(surface, x, newY, width, height, color);
}

b2Body *CPipe::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter((float)this->x + (this->width / 2)), pixelToMeter((float)this->y - (this->height / 2)));

	newBody = world->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(pixelToMeter((float)this->width / 2), pixelToMeter((float)this->height / 2));
	b2Fixture *fixture = newBody->CreateFixture(&box, 0);

	// Set collision properties
	b2Filter filter;
	filter.categoryBits = PIPE_CATEGORY;
	filter.maskBits = PIPE_MASK;
	filter.groupIndex = 0;
	fixture->SetFilterData(filter);

	return newBody;
}