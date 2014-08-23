#include <list>
#include <exception>

#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Pipe.h"

CPipe::CPipe() :
	CAbstractBody(),
	width(100),
	height(100),
	frame(0),
	drops()
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height) :
	CAbstractBody(x, y),
	width(width),
	height(height),
	frame(0),
	drops()
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height, Uint32 color) :
	CAbstractBody(x, y, color),
	width(width),
	height(height),
	frame(0),
	drops()
{
	this->body = this->createBody();
}

CPipe::CPipe(const CPipe &copy) :
	CAbstractBody(copy),
	width(copy.width),
	height(copy.height),
	frame(copy.frame),
	drops(copy.drops)
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
		this->frame = copy.frame;
		this->drops = copy.drops; // Probably has some problems
		throw new std::exception(); // Throw to let me know if the code ever reaches here
	}

	return *this;
}

CPipe::~CPipe()
{
	drops.clear();
}

void CPipe::draw(SDL_Surface *surface, unsigned int frame) const
{
	int newY = flipYAxis(this->y);
	Draw_FillRect(surface, x, newY-height, width, height, color);
}

void CPipe::captureDrop(CDrop &drop, unsigned int frame)
{

}

// For now, this immediately emits the drops it captures. Eventually it will hold them for a few frames to simulate moving through the pipe.
void CPipe::captureDrops(std::list<CDrop> &drops, unsigned int frame)
{
	std::list<CDrop> dropsToReAdd;

	std::list<CDrop>::iterator drop = drops.begin();
	while (drop != drops.end()) {
		if (((drop->getX() - drop->getRadius()) >= this->x) &&
		    ((drop->getX() + drop->getRadius()) < (this->x + this->width)) &&
		    ((drop->getY() - drop->getRadius()) >= this->y) &&
		    ((drop->getY() + drop->getRadius()) < (this->y + this->height))) {
				// Create copy when adding to list. Place it one pixel outside the box so it doesn't get captured again.
				int newX = this->x - 1;
				int newY = this->y + (this->height / 2);
				b2Vec2 newVelocity = b2Vec2(-4.0f, 4.0f);
				dropsToReAdd.push_back(CDrop(newX, newY, drop->getRadius(), newVelocity, drop->getColor()));
				drops.erase(drop++);
		}
		else {
			++drop;
		}
	}

	for (std::list<CDrop>::iterator drop = dropsToReAdd.begin(); drop != dropsToReAdd.end(); ++drop) {
		drops.push_back(*drop);
	}
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