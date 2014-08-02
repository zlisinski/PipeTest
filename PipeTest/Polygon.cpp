#include "Polygon.h"

#include "SDL.h"
#include "SDL_draw.h"
#include "SDL_gfxPrimitives.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Polygon.h"


CPolygon::CPolygon() :
	CAbstractBody(),
	vertexCount(0),
	xs(NULL),
	ys(NULL),
	b2Vertices(NULL)
{
	// This will probably crash without vertices...
	this->calculateXY();
	this->calculateB2Vertices();
	this->body = this->createBody();
}

CPolygon::CPolygon(int xs[], int ys[], int vertexCount) :
	CAbstractBody(),
	vertexCount(vertexCount),
	xs(NULL),
	ys(NULL),
	b2Vertices(NULL)
{
	this->xs = new int[vertexCount];
	this->ys = new int[vertexCount];
	memcpy(this->xs, xs, sizeof(int) * this->vertexCount);
	memcpy(this->ys, ys, sizeof(int) * this->vertexCount);

	this->color = this->randomColor();

	this->calculateXY();
	this->calculateB2Vertices();

	this->body = this->createBody();
}

CPolygon::CPolygon(int xs[], int ys[], int vertexCount, Uint32 color) :
	CAbstractBody(),
	vertexCount(vertexCount),
	xs(NULL),
	ys(NULL),
	b2Vertices(NULL)
{
	this->xs = new int[vertexCount];
	this->ys = new int[vertexCount];
	memcpy(this->xs, xs, sizeof(int) * vertexCount);
	memcpy(this->ys, ys, sizeof(int) * vertexCount);

	this->color = color;

	this->calculateXY();
	this->calculateB2Vertices();

	this->body = this->createBody();
}

CPolygon::CPolygon(const CPolygon &copy) :
	CAbstractBody(copy.x, copy.y, copy.color)
{
	this->vertexCount = copy.vertexCount;
	this->xs = new int[vertexCount];
	this->ys = new int[vertexCount];
	memcpy(this->xs, copy.xs, sizeof(int) * this->vertexCount);
	memcpy(this->ys, copy.ys, sizeof(int) * this->vertexCount);

	this->color = copy.color;

	this->calculateXY();
	this->calculateB2Vertices();

	this->body = copy.createBody();
}

CPolygon &CPolygon::operator=(const CPolygon &copy)
{
	if (this != &copy) {
		this->x = copy.x;
		this->y = copy.y;
		this->vertexCount = copy.vertexCount;

		delete [] this->xs;
		delete [] this->ys;
		delete [] this->b2Vertices;
		this->xs = new int[vertexCount];
		this->ys = new int[vertexCount];
		this->b2Vertices = new b2Vec2[vertexCount];
		memcpy(this->xs, copy.xs, sizeof(int) * this->vertexCount);
		memcpy(this->ys, copy.ys, sizeof(int) * this->vertexCount);
		memcpy(this->b2Vertices, copy.b2Vertices, sizeof(b2Vec2) * this->vertexCount);

		this->color = copy.color;
		this->body = copy.createBody();
	}

	return *this;
}

CPolygon::~CPolygon()
{
	delete [] this->xs;
	delete [] this->ys;
	delete [] this->b2Vertices;
}

void CPolygon::draw(SDL_Surface *surface) const
{
	// Flip Y axis because SDL is Y-down while the rest of the code is Y-up. Also convert Sint32 to Sint16.
	Sint16 *flippedYs = new Sint16[this->vertexCount];
	for (int i = 0; i < this->vertexCount; i++) {
		flippedYs[i] = flipYAxis(this->ys[i]);
	}

	// This is necessary because filledPolygonColor() wants an array of Sint16s and this->xs is an array of Sint32s.
	// Casting this->xs to Sint16 doesn't work, because then this->xs[1] == 0.
	Sint16 *Xs = new Sint16[this->vertexCount];
	for (int i = 0; i < this->vertexCount; i++) {
		Xs[i] = this->xs[i];
	}
	
	// Switch ARGB to RGBA
	Uint32 newColor = (this->color << 8) + 255;

	filledPolygonColor(surface, Xs, flippedYs, this->vertexCount, newColor);

	delete [] flippedYs;
	delete [] Xs;
}

b2Body *CPolygon::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter((float)this->x), pixelToMeter((float)this->y));
	
	newBody = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.Set(this->b2Vertices, this->vertexCount);

	// Set collision properties
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.filter.categoryBits = WALL_CATEGORY;
	fixtureDef.filter.maskBits = WALL_MASK;
	fixtureDef.filter.groupIndex = 0;

	newBody->CreateFixture(&fixtureDef);

	return newBody;
}

// Select the lowest(bottom) Y point which is left-most(lowest X) as the origin
void CPolygon::calculateXY()
{
	int originIndex = 0;

	for (int i = 0; i < this->vertexCount; i++) {
		if (this->ys[i] <= this->ys[originIndex]) {
			if (this->ys[i] == this->ys[originIndex] && this->xs[i] <= this->xs[originIndex]) {
				originIndex = i;
			}
		}
	}

	// Swap the origin point into the 0 index
	if (originIndex != 0) {
		int tempX = this->xs[0];
		int tempY = this->ys[0];
		this->xs[0] = this->xs[originIndex];
		this->ys[0] = this->ys[originIndex];
		this->xs[originIndex] = tempX;
		this->ys[originIndex] = tempY;
	}

	this->x = this->xs[0];
	this->y = this->ys[0];
}

// Convert vertex point units to be b2Vec2 points relative to the origin point
void CPolygon::calculateB2Vertices()
{
	delete [] this->b2Vertices;
	this->b2Vertices = new b2Vec2[this->vertexCount];

	// The first vertex is (this->x, this->y)
	b2Vec2 origin = b2Vec2(pixelToMeter((float)this->x), pixelToMeter((float)this->y));

	// Subtract the origin vertex from each vertext
	for (int i = 0; i < this->vertexCount; i++) {
		b2Vec2 vec = b2Vec2(pixelToMeter((float)this->xs[i]), pixelToMeter((float)this->ys[i]));
		this->b2Vertices[i] = vec - origin;
	}
}