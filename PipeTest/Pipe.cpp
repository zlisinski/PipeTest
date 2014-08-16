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
	Draw_FillRect(surface, x, newY-height, width, height, color);
}

b2Body *CPipe::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter((float)this->x), pixelToMeter((float)this->y));

	newBody = world->CreateBody(&bodyDef);

	int xs[4] = {this->x, this->x, this->x + this->width, this->x + this->width};
	int ys[4] = {this->y, this->y + this->height, this->y + this->height, this->y};
	b2Vec2 *b2Vertices = new b2Vec2[4];

	// The first vertex is (this->x, this->y)
	b2Vec2 origin = b2Vec2(pixelToMeter((float)this->x), pixelToMeter((float)this->y));

	// Subtract the origin vertex from each vertext
	for (int i = 0; i < 4; i++) {
		b2Vec2 vec = b2Vec2(pixelToMeter((float)xs[i]), pixelToMeter((float)ys[i]));
		b2Vertices[i] = vec - origin;
	}

	b2PolygonShape shape;
	shape.Set(b2Vertices, 4);

	delete [] b2Vertices;

	// Set collision properties
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.filter.categoryBits = PIPE_CATEGORY;
	fixtureDef.filter.maskBits = PIPE_MASK;
	fixtureDef.filter.groupIndex = 0;

	newBody->CreateFixture(&fixtureDef);

	return newBody;
}