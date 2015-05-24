#include <list>
#include <exception>

#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Pipe.h"
#include "Drop.h"

CPipe::CPipe() :
	CAbstractBody(),
	width(100),
	height(100),
	frame(0),
	drops(),
	captureSide(Top),
	emitSide(Left)
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height, ESide captureSide, ESide emitSide) :
	CAbstractBody(x, y),
	width(width),
	height(height),
	frame(0),
	drops(),
	captureSide(captureSide),
	emitSide(emitSide)
{
	this->body = this->createBody();
}

CPipe::CPipe(int x, int y, int width, int height, ESide captureSide, ESide emitSide, Uint32 color) :
	CAbstractBody(x, y, color),
	width(width),
	height(height),
	frame(0),
	drops(),
	captureSide(captureSide),
	emitSide(emitSide)
{
	this->body = this->createBody();
}

CPipe::CPipe(const CPipe &copy) :
	CAbstractBody(copy),
	width(copy.width),
	height(copy.height),
	frame(copy.frame),
	drops(copy.drops),
	captureSide(copy.captureSide),
	emitSide(copy.emitSide)
{
	this->body = copy.createBody();

	// I don't remember why createBody is called on the copy and not on this object.
	// Because of this, we have to set the UserData here, instead of in createBody.
	b2Fixture *f = this->body->GetFixtureList();
	while (f != NULL) {
		f->SetUserData((void*)this);
		f = f->GetNext();
	}
	//this->body->SetUserData((void*)this);
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
		this->captureSide = copy.captureSide;
		this->emitSide = copy.emitSide;
		this->drops = copy.drops; // Probably has some problems
		throw std::exception(); // Throw to let me know if the code ever reaches here

		// I don't remember why createBody is called on the copy and not on this object.
		// Because of this, we have to set the UserData here, instead of in createBody.
		b2Fixture *f = this->body->GetFixtureList();
		while (f != NULL) {
			f->SetUserData((void*)this);
			f = f->GetNext();
		}
		//this->body->SetUserData((void*)this);
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

// Checks if a drop can collide with this pipe based on the captureSide member. A drop will collide with
// all sides except for the the captureSide. Using the top side as an example, If the drop is within the
// bounds of the left and right side and its bottom is above the bottom of the pipe, it is considered as
// having come from the top, and will not collide. This obviously has some flaws in the event that it
// completely passes a side in one time step.
bool CPipe::allowCollision(CDrop &drop)
{
	int dropMinX = drop.getMinX();
	int dropMaxX = drop.getMaxX();
	int dropMinY = drop.getMinY();
	int dropMaxY = drop.getMaxY();
	int pipeMinX = this->getMinX();
	int pipeMaxX = this->getMaxX();
	int pipeMinY = this->getMinY();
	int pipeMaxY = this->getMaxY();

	switch (this->captureSide) {
		case Top:
			if ((dropMinX >= pipeMinX) &&
			    (dropMaxX < pipeMaxX) &&
			    (dropMinY >= pipeMinY))
					return false;
			break;
		case Right:
			if ((dropMinX >= pipeMinX) &&
			    (dropMinY >= pipeMinY) &&
			    (dropMaxY < pipeMaxY))
					return false;
			break;
		case Bottom:
			if ((dropMinX >= dropMinX) &&
			    (dropMaxX < dropMaxX) &&
			    (dropMaxY < pipeMaxY))
					return false;
			break;
		case Left:
			if ((dropMaxX < pipeMaxX) &&
			    (dropMinY >= pipeMinY) &&
			    (dropMaxY < pipeMaxY))
					return false;
			break;
	}

	return true;
}

// If the drop is completely within the pipe, it is considered "Captured". This means it gets added to 
// an internal list and will be removed from the global drops list in main.cpp. Eventually we will use
// the frame count to hold the drop for a few frames to simulate travel time inside the pipe.
bool CPipe::captureDrop(CDrop &drop, unsigned int frame)
{
	if (drop.getMinX() >= this->getMinX() &&
	    drop.getMaxX() < this->getMaxX() &&
	    drop.getMinY() >= this->getMinY() &&
	    drop.getMaxY() < this->getMaxY()) {
			// Set drop as captured so we can remove it from the main list of drops in main.cpp
			drop.setCaptured(true);
			this->drops.push_back(&drop);
			return true;
	}

	return false;
}

// Returns a list of drops to emit from the pipe. These are drops that were previously captured.
// Eventually we will use the frame count to hold the drop for a few frames to simulate travel time
// inside the pipe.
// Right now it only emits on left side.
std::list<CDrop *> CPipe::emitDrops(unsigned int frame)
{
	std::list<CDrop *> dropsToEmit;

	std::list<CDrop *>::iterator ppDrop = drops.begin();
	while (ppDrop != drops.end()) {
		CDrop *pDrop = *ppDrop;

		//if (frame) {
			// Place drop one pixel outside the pipe so it doesn't get captured again.
			int newX = (this->x - 1) + (pDrop->getRadius() / 2) + (rand() % CDrop::randPos);
			int newY = this->y + (this->height / 2) + (rand() % CDrop::randPos);

			b2Vec2 newVelocity = b2Vec2(-4.0f, 4.0f);

			pDrop->move(newX, newY);
			pDrop->getBody()->SetLinearVelocity(newVelocity);

			pDrop->setCaptured(false);
			dropsToEmit.push_back(pDrop);
			drops.erase(ppDrop++);
		//}
	}

	return dropsToEmit;
}

b2Body *CPipe::createBody() const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter(this->x), pixelToMeter(this->y));

	newBody = world->CreateBody(&bodyDef);

	int xs[4] = {this->x, this->x, this->x + this->width, this->x + this->width};
	int ys[4] = {this->y, this->y + this->height, this->y + this->height, this->y};
	b2Vec2 *b2Vertices = new b2Vec2[4];

	// The first vertex is (this->x, this->y)
	b2Vec2 origin = b2Vec2FromPixel(this->x, this->y);

	// Subtract the origin vertex from each vertext
	for (int i = 0; i < 4; i++) {
		b2Vec2 vec = b2Vec2FromPixel(xs[i], ys[i]);
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

	// Set userData
	fixtureDef.userData = (void*)this;

	newBody->CreateFixture(&fixtureDef);
	//newBody->SetUserData((void*)this);

	return newBody;
}
