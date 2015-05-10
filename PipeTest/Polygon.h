#pragma once
#include "abstractbody.h"

class CPolygon : public CAbstractBody
{
public:
	CPolygon();
	CPolygon(int xs[], int ys[], int vertexCount);               // Create a polygon with a random color
	CPolygon(int xs[], int ys[], int vertexCount, Uint32 color); // Create a polygon with the specified color
	CPolygon(int x, int y, int width, int height);               // Create a rectangle with a random color. x,y is bottom left point
	CPolygon(int x, int y, int width, int height, Uint32 color); // Create a rectangle with the specified color. x,y is bottom left point
	CPolygon(const CPolygon &copy);
	CPolygon &operator=(const CPolygon &copy);
	virtual ~CPolygon();

	/// Move the Polygon to the specified position.
	virtual void move(Uint32 x, Uint32 y);

	/// Rotate the Polygon by the specified number of degrees.
	void rotate(float degrees);

	/// Updates local data from Box2d data.
	virtual void update();

	virtual void draw(SDL_Surface *surface, unsigned int frame) const;

	/// Gets the index of the nearest vertex to the specified pixel position, but only if it's within maxPixelDistance pixels. Otherwise returns -1.
	int getNearestVertextIndex(Uint32 x, Uint32 y, Uint32 maxPixelDistance = 10) const;

	/// Returns the pixel coordinates of the vertex specified by vertexIndex.
	b2Vec2 getVertex(int vertexIndex) const;

	/// Moves the specified vertex to the specified pixel position.
	void moveVertex(int vertexIndex, Uint32 x, Uint32 y);

protected:
	int vertexCount;
	int *xs;
	int *ys;
	b2PolygonShape shape;

	void calculateXY();

private:
	void constructPolygon(int xs[], int ys[], int vertexCount, Uint32 color);
	void constructRectangle(int x, int y, int width, int height, Uint32 color);

	/// Creates this->shape from points in this->xs and this->ys, and then reorder the points in
	/// this->xs and this->ys to match the order in the box2d fixture.
	void createShape();

	b2Body *createBody() const;
	static b2FixtureDef createFixtureDef(const b2Shape *shape, void *userData);
};