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

	virtual void draw(SDL_Surface *surface, unsigned int frame) const;

protected:
	int vertexCount;
	int *xs;
	int *ys;
	b2Vec2 *b2Vertices;

	void calculateXY();
	void calculateB2Vertices();

private:
	void constructPolygon(int xs[], int ys[], int vertexCount, Uint32 color);
	void constructRectangle(int x, int y, int width, int height, Uint32 color);

	b2Body *createBody() const;
};