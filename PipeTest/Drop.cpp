#include "SDL.h"
#include "SDL_draw.h"

#include "main.h"
#include "Drop.h"

CDrop::CDrop() :
	CAbstractBody(),
	captured(false)
{
	this->radius = 5 + (rand() % 10);
	this->x = SCREEN_WIDTH - 50 + (rand() % 20);
	this->y = 668 + (rand() % 20);
	this->color = this->randomColor();
	this->body = this->createBody(b2Vec2(-4.0f, 0.0f));
}

CDrop::CDrop(int x, int y, int radius, const b2Vec2 &velocity) :
	CAbstractBody(x, y),
	radius(radius),
	captured(false)
{
	this->color = this->randomColor();
	this->body = this->createBody(velocity);
}

CDrop::CDrop(int x, int y, int radius, const b2Vec2 &velocity, Uint32 color) :
	CAbstractBody(x, y, color),
	radius(radius),
	captured(false)
{
	this->body = this->createBody(velocity);
}

CDrop::CDrop(const CDrop &copy) :
	CAbstractBody(copy.x, copy.y, copy.color),
	radius(copy.radius),
	captured(copy.captured)
{
	this->body = copy.createBody(copy.getVelocity());

	// I don't remember why createBody is called on the copy and not on this object.
	// Because of this, we have to set the UserData here, instead of in createBody.
	b2Fixture *f = this->body->GetFixtureList();
	while (f != NULL) {
		f->SetUserData((void*)this);
		f = f->GetNext();
	}
	//this->body->SetUserData((void*)this);
}

CDrop &CDrop::operator=(const CDrop &copy)
{
	if (this != &copy) {
		this->radius = copy.radius;
		this->x = copy.x;
		this->y = copy.y;
		this->color = copy.color;
		this->captured = copy.captured;
		this->body = copy.createBody(copy.getVelocity());

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

CDrop::~CDrop()
{
}

void CDrop::draw(SDL_Surface *surface, unsigned int frame) const
{
	// Flip Y axis because SDL is Y-down while the rest of the code is Y-up
	int newY = flipYAxis(this->y);
	Draw_FillCircle(surface, this->x, newY, this->radius, this->color);
}

b2Body *CDrop::createBody(const b2Vec2 &velocity) const
{
	b2Body *newBody;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(pixelToMeter((float)this->x), pixelToMeter((float)this->y));
	bodyDef.linearVelocity.Set(velocity.x, velocity.y);
	
	newBody = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = pixelToMeter((float)this->radius);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.25f;
	
	// Set collision properties
	fixtureDef.filter.categoryBits = DROP_CATEGORY;
	fixtureDef.filter.maskBits = DROP_MASK;
	fixtureDef.filter.groupIndex = 0;

	// Set userData
	fixtureDef.userData = (void*)this;

	newBody->CreateFixture(&fixtureDef);

	return newBody;
}

Uint32 CDrop::randomColor() const {
	// Make a random green color
	Uint32 r = rand() % 128;
	Uint32 g = 128 + (rand() % 128);
	Uint32 b = rand() % 128;
	return rgb(r, g, b);
}