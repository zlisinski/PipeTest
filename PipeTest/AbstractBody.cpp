#include "main.h"
#include "AbstractBody.h"

CAbstractBody::CAbstractBody() :
	x(0),
	y(0),
	color(0xFFFFFFFF),
	body(NULL)
{
}

CAbstractBody::CAbstractBody(int x, int y) :
	x(x),
	y(y),
	body(NULL)
{
	this->color = this->randomColor();
}

CAbstractBody::CAbstractBody(int x, int y, Uint32 color) :
	x(x),
	y(y),
	color(color),
	body(NULL)
{
}

CAbstractBody::CAbstractBody(const CAbstractBody &copy) :
	x(copy.x),
	y(copy.y),
	color(copy.color),
	body(NULL)
{
}

CAbstractBody &CAbstractBody::operator=(const CAbstractBody &copy)
{
	if (this != &copy) {
		this->x = copy.x;
		this->y = copy.y;
		this->color = copy.color;
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

	b2Vec2 newPos = b2Vec2(pixelToMeter(x), pixelToMeter(y));
	this->body->SetTransform(newPos, 0);
}

void CAbstractBody::update()
{
	b2Vec2 pos = this->body->GetPosition();
	this->x = meterToPixel(pos.x);
	this->y = meterToPixel(pos.y);
}

bool CAbstractBody::testPoint(Uint32 x, Uint32 y)
{
	if (this->body == NULL)
		return false;

	b2Vec2 point = b2Vec2(pixelToMeter((float)x), pixelToMeter((float)y));

	b2Fixture *fixture = this->body->GetFixtureList();
	while (fixture != NULL) {
		if (fixture->TestPoint(point))
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

Uint32 CAbstractBody::randomColor() const {
	const Uint8 min = 64;

	Uint8 r = min + (rand() % (256 - min));
	Uint8 g = min + (rand() % (256 - min));
	Uint8 b = min + (rand() % (256 - min));

	return rgb(r, g, b);
}