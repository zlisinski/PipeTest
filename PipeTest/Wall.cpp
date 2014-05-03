#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"
#include "main.h"
#include "Wall.h"

CWall::CWall() :
	CAbstractBody(),
	width(100),
	height(100)
{
	this->body = this->createBody();
}

CWall::CWall(int x, int y, int width, int height, Uint32 color) :
	CAbstractBody(x, y, color),
	width(width),
	height(height)
{
	this->body = this->createBody();
}

CWall::CWall(const CWall &copy) :
	CAbstractBody(copy),
	width(copy.width),
	height(copy.height)
{
	this->body = copy.createBody();
}

CWall &CWall::operator=(const CWall &copy)
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

CWall::~CWall()
{
}

void CWall::draw(SDL_Surface *surface) const
{
	int newY = flipYAxis(this->y);
	Draw_FillRect(surface, x, newY, width, height, color);
}

b2Body *CWall::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter((float)this->x + (this->width / 2)), pixelToMeter((float)this->y - (this->height / 2)));

	newBody = world->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(pixelToMeter((float)this->width / 2), pixelToMeter((float)this->height / 2));
	newBody->CreateFixture(&box, 0);

	return newBody;
}