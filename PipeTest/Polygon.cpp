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
	ys(NULL)
{
	// This will probably crash without vertices...
	this->createShape();
	this->body = this->createBody();
}

// Create a polygon with a random color
CPolygon::CPolygon(int xs[], int ys[], int vertexCount) :
	CAbstractBody(),
	vertexCount(vertexCount),
	xs(NULL),
	ys(NULL)
{
	this->constructPolygon(xs, ys, vertexCount, this->randomColor());
}

// Create a polygon with the specified color
CPolygon::CPolygon(int xs[], int ys[], int vertexCount, Uint32 color) :
	CAbstractBody(),
	vertexCount(vertexCount),
	xs(NULL),
	ys(NULL)
{
	this->constructPolygon(xs, ys, vertexCount, color);
}

// Create a rectangle with a random color
CPolygon::CPolygon(int x, int y, int width, int height) :
	CAbstractBody(),
	vertexCount(4)
{
	this->constructRectangle(x, y, width, height, this->randomColor());
}

// Create a rectangle with the specified color
CPolygon::CPolygon(int x, int y, int width, int height, Uint32 color) :
	CAbstractBody(),
	vertexCount(4)
{
	this->constructRectangle(x, y, width, height, color);
}

// Called from constructor to create a polygon
void CPolygon::constructPolygon(int xs[], int ys[], int vertexCount, Uint32 color)
{
	// Make sure we have between 3 and 8 vertices
	if (vertexCount <= 2 || vertexCount > b2_maxPolygonVertices)
		throw new std::out_of_range("vertexCount must be between [3, b2_maxPolygonVertices]");

	this->xs = new int[vertexCount];
	this->ys = new int[vertexCount];
	memcpy(this->xs, xs, sizeof(int) * vertexCount);
	memcpy(this->ys, ys, sizeof(int) * vertexCount);

	this->color = color;

	// Create this->shape, and reorder the points in this->xs and this->ys to match the order in the box2d fixture.
	this->createShape();
	this->body = this->createBody();
}

// Called from constructor to create a rectangle
void CPolygon::constructRectangle(int x, int y, int width, int height, Uint32 color)
{
	// Set vertices clockwise from the bottom left
	this->xs = new int[4];
	this->xs[0] = x;
	this->xs[1] = x;
	this->xs[2] = x + width;
	this->xs[3] = x + width;

	this->ys = new int[4];
	this->ys[0] = y;
	this->ys[1] = y + height;
	this->ys[2] = y + height;
	this->ys[3] = y;

	this->color = color;

	// Create this->shape, and reorder the points in this->xs and this->ys to match the order in the box2d fixture.
	this->createShape();
	this->body = this->createBody();
}

CPolygon::CPolygon(const CPolygon &copy) :
	CAbstractBody(copy.x, copy.y, copy.color)
{
	this->vertexCount = copy.vertexCount;
	this->xs = new int[vertexCount];
	this->ys = new int[vertexCount];
	memcpy(this->xs, copy.xs, sizeof(*this->xs) * this->vertexCount);
	memcpy(this->ys, copy.ys, sizeof(*this->ys) * this->vertexCount);

	this->color = copy.color;

	this->shape = copy.shape;
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
		this->xs = new int[vertexCount];
		this->ys = new int[vertexCount];
		memcpy(this->xs, copy.xs, sizeof(*this->xs) * this->vertexCount);
		memcpy(this->ys, copy.ys, sizeof(*this->ys) * this->vertexCount);

		this->color = copy.color;

		this->shape = copy.shape;
		this->body = copy.createBody();
	}

	return *this;
}

CPolygon::~CPolygon()
{
	delete [] this->xs;
	delete [] this->ys;
}

/// Move the Polygon to the specified position.
void CPolygon::move(Uint32 x, Uint32 y)
{
	// Create vector from new position.
	b2Vec2 newPos = b2Vec2FromPixel(x, y);

	// Move Box2d body to specified position.
	this->body->SetTransform(newPos, this->body->GetAngle());

	// Get updated vertices from Box2d.
	this->update();

	// Update shape. This should not be needed.
	//this->createShape();
}

/// Rotate the Polygon by the specified number of degrees.
// This first rotates the body around x,y which is one of the vertices, then it moves the body by the
// difference of the center before and after the rotation, so that in the end it rotates around the center.
// I feel like there has to be a better way to do this, but this works for now.
void CPolygon::rotate(float degrees)
{
	// Get current angle of body in radians.
	float bodyAngleRad = this->body->GetAngle();

	// Convert angle to rotate to from degrees to radians.
	float rad = degreeToRadian(degrees);

	// Get the center of the polygon before rotating.
	b2Vec2 centerBefore = this->getCenter();

	// Rotate the body. This rotates around the origin, not the center. Later we'll move it too.
	this->body->SetTransform(this->body->GetPosition(), bodyAngleRad + rad);

	// Get the center of the polygon before rotating.
	b2Vec2 centerAfter = this->getCenter();

	// Move body so that it really rotates around center.
	b2Vec2 newPos = this->body->GetPosition() + centerBefore - centerAfter;
	this->body->SetTransform(newPos, this->body->GetAngle());

	// Update this->xs and this->ys points.
	this->update();
}

/// Updates local data from Box2d data.
void CPolygon::update()
{
	// Updates this->x and this->y
	CAbstractBody::update();

	this->shape = this->getShapeFromBody();

	// Update all vertices from the Box2d body.
	for (int i = 0; i < this->vertexCount; i++) {
		// Convert local point to world coordinates.
		b2Vec2 worldVec = this->body->GetWorldPoint(this->shape.m_vertices[i]);

		this->xs[i] = meterToPixel(worldVec.x);
		this->ys[i] = meterToPixel(worldVec.y);
	}
}

/// Draw the polygon.
void CPolygon::draw(SDL_Surface *surface, unsigned int frame) const
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

	// Draw a small circle at the center of the body.
	b2Vec2 center = this->getCenter();
	Sint16 x = meterToPixel(center.x);
	Sint16 y = flipYAxis(meterToPixel(center.y));
	filledCircleColor(surface, x, y, 3, 0xFF0000FF);
}

/// Gets the index of the nearest vertex to the specified pixel position, but only if it's within maxPixelDistance pixels. Otherwise returns -1.
int CPolygon::getNearestVertextIndex(Uint32 x, Uint32 y, Uint32 maxPixelDistance) const
{
	b2Vec2 point = b2Vec2FromPixel(x, y);
	b2Vec2 bestDelta(0, 0);
	int bestIndex = -1;

	// Find the vertex nearest the x,y point.
	for (int i = 0; i < this->vertexCount; i++) {
		b2Vec2 vertex = b2Vec2FromPixel(this->xs[i], this->ys[i]);
		b2Vec2 delta = vertex - point;

		// LengthSquared() is faster than Length() because Length() uses square root.
		if (bestIndex == -1 || delta.LengthSquared() < bestDelta.LengthSquared()) {
			bestIndex = i;
			bestDelta = delta;
		}
	}

	debugPrint("i=%d, x=%d, y=%d", bestIndex, meterToPixel(bestDelta.x), meterToPixel(bestDelta.y));

	// Check that the nearest vertex is within maxPixelDistance.
	if (abs(bestDelta.x) <= pixelToMeter(maxPixelDistance) &&
		abs(bestDelta.y) <= pixelToMeter(maxPixelDistance)) {
			return bestIndex;
	}

	return -1;
}

/// Returns the pixel coordinates of the vertex specified by vertexIndex.
b2Vec2 CPolygon::getVertex(int vertexIndex) const
{
	if (vertexIndex < 0 || vertexIndex >= this->vertexCount)
		throw new std::out_of_range("Bad vertexIndex");

	// Returns a b2Vec2 using pixel values, not the actual Box2d vertex. This is kinda confusing.
	return b2Vec2FromPixel(this->xs[vertexIndex], this->ys[vertexIndex]);
}

/// Moves the specified vertex to the specified pixel position.
/// Returns the new index of the vertex in the case that the vertex index changes as a result of moving.
int CPolygon::moveVertex(int vertexIndex, Uint32 x, Uint32 y)
{
	if (vertexIndex < 0 || vertexIndex >= this->vertexCount)
		throw new std::out_of_range("Bad vertexIndex");

	// Update the vertex
	this->xs[vertexIndex] = x;
	this->ys[vertexIndex] = y;

	// Delete old fixture, there currently is only one fixture.
	b2Fixture *f = this->body->GetFixtureList();
	if (f == NULL)
		throw new std::invalid_argument("fixture is NULL");
	this->body->DestroyFixture(f);

	// Create shape, and update this->xs and this->ys.
	this->createShape();

	// Create new fixture
	b2FixtureDef fixtureDef = CPolygon::createFixtureDef(&this->shape, (void *)this);
	this->body->CreateFixture(&fixtureDef);

	// Because we are moving a vertex, box2d may change the order of vertices.
	// Find the index of the same vertex, so we can return it to the controlling code.
	for (int i = 0; i < this->vertexCount; i++) {
		if (x == this->xs[i] && y == this->ys[i]) {
			return i;
		}
	}

	throw new std::exception("Can't find new index after moving vertex");
}

/// Creates this->shape from points in this->xs and this->ys, and then reorder the points in
/// this->xs and this->ys to match the order in the box2d shape.
void CPolygon::createShape()
{
	b2Vec2 *b2Vertices = new b2Vec2[this->vertexCount];

	// Set this->x and this->y to the bottom-left-most vertex.
	this->calculateXY();

	// The first vertex is (this->x, this->y)
	b2Vec2 origin = b2Vec2FromPixel(this->x, this->y);

	// We can't use b2Body::GetLocalPoint, as the body may not exist yet, so manually subtract the origin from the x,y points.
	for (int i = 0; i < this->vertexCount; i++) {
		b2Vec2 vec = b2Vec2FromPixel(this->xs[i], this->ys[i]);
		b2Vertices[i] = vec - origin;
	}

	// Set the shape. This reorders the vertices into a convex shape, if the points are out of order.
	this->shape.Set(b2Vertices, this->vertexCount);

	// Update our this->xs and this->ys arrays to match the Box2d vertices. By using Box2d's vertex order, we are assured that 
	// the order of the vertices creates a convex shape when passing the vertices to the SDL draw functions.
	for (int i = 0; i < this->vertexCount; i++) {
		this->xs[i] = meterToPixel(this->shape.m_vertices[i].x + origin.x);
		this->ys[i] = meterToPixel(this->shape.m_vertices[i].y + origin.y);
	}
}

b2Body *CPolygon::createBody() const
{
	b2Body *newBody;

	// Create body.
	b2BodyDef bodyDef;
	bodyDef.position.Set(pixelToMeter(this->x), pixelToMeter(this->y));
	bodyDef.type = b2_staticBody;
	newBody = world->CreateBody(&bodyDef);

	// Create fixture
	b2FixtureDef fixtureDef = CPolygon::createFixtureDef(&this->shape, (void *)this);
	newBody->CreateFixture(&fixtureDef);

	//newBody->SetUserData((void*)this);

	return newBody;
}

b2FixtureDef CPolygon::createFixtureDef(const b2Shape *shape, void *userData)
{
	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;

	// Set collision properties
	fixtureDef.filter.categoryBits = WALL_CATEGORY;
	fixtureDef.filter.maskBits = WALL_MASK;
	fixtureDef.filter.groupIndex = 0;

	// Set userData
	fixtureDef.userData = userData;

	return fixtureDef;
}

/// Select the lowest(bottom) Y point which is left-most(lowest X) as the origin.
void CPolygon::calculateXY()
{
	int originIndex = 0;

	for (int i = 1; i < this->vertexCount; i++) {
		// Find Bottom-Left-most point.
		if (this->xs[i] < this->xs[originIndex] ||
			(this->xs[i] == this->xs[originIndex] && this->ys[i] < this->ys[originIndex])) {
				originIndex = i;
		}
	}

	//debugPrint("originIndex=%d", originIndex);

	this->x = this->xs[originIndex];
	this->y = this->ys[originIndex];
}

/// Prints out the vertices to debug output.
void CPolygon::debugVertices(const std::string &label) const
{
	// There is currently only one fixture. Update this if we add support for more.
	b2Fixture *f = this->body->GetFixtureList();
	if (f == NULL)
		throw new std::invalid_argument("Fixture is NULL");

	// This shouldn't be anything but a b2PolygonShape.
	b2PolygonShape *s = dynamic_cast<b2PolygonShape *>(f->GetShape());
	if (s == NULL)
		throw new std::bad_cast("shape is not a b2PolygonShape");

	// Print all vertices.
	for (int i = 0; i < this->vertexCount; i++) {
		b2Vec2 worldVec = this->body->GetWorldPoint(s->m_vertices[i]);

		int x = meterToPixel(worldVec.x);
		int y = meterToPixel(worldVec.y);

		debugPrint("%s %d=%d,%d", label.c_str(), i, x, y);
	}
}

/// Gets the b2PolygonShape object from the body.
b2PolygonShape CPolygon::getShapeFromBody() const
{
	// There is currently only one fixture. Update this if we add support for more.
	b2Fixture *f = this->body->GetFixtureList();
	if (f == NULL)
		throw new std::invalid_argument("Fixture is NULL");

	// This shouldn't be anything but a b2PolygonShape.
	b2PolygonShape *s = dynamic_cast<b2PolygonShape *>(f->GetShape());
	if (s == NULL)
		throw new std::bad_cast("shape is not a b2PolygonShape");

	return *s;
}

/// Gets the center of the polygon.
// Static bodies don't have mass, so we can't use the center of mass functions.
b2Vec2 CPolygon::getCenter() const
{
	b2Vec2 center = this->body->GetWorldPoint(this->shape.m_centroid);

	return center;
}
