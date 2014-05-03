#include "main.h"
#include "AbstractBody.h"

CAbstractBody::CAbstractBody() :
	x(0),
	y(0),
	color(0xFFFFFFFF),
	body(NULL)
{
	//this->body = this->createBody();
}

CAbstractBody::CAbstractBody(int x, int y, Uint32 color) :
	x(x),
	y(y),
	color(color),
	body(NULL)
{
	//this->body = this->createBody();
}

CAbstractBody::CAbstractBody(const CAbstractBody &copy) :
	x(copy.x),
	y(copy.y),
	color(copy.color),
	body(NULL)
{
	//this->body = copy.createBody();
}

CAbstractBody &CAbstractBody::operator=(const CAbstractBody &copy)
{
	if (this != &copy) {
		this->x = copy.x;
		this->y = copy.y;
		this->color = copy.color;
		this->body = copy.createBody();
	}

	return *this;
}

CAbstractBody::~CAbstractBody()
{
	if (this->body != NULL)
		world->DestroyBody(this->body);
}

void CAbstractBody::move(Uint32 x, Uint32 y)
{
	this->x = x;
	this->y = y;
}

void CAbstractBody::update()
{
	b2Vec2 pos = this->body->GetPosition();
	this->x = meterToPixel(pos.x);
	this->y = meterToPixel(pos.y);
}
