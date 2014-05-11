#pragma once
#include "abstractbody.h"

class CPolygon : public CAbstractBody
{
public:
	CPolygon();
	CPolygon(int xs[], int ys[], int vertexCount);
	CPolygon(int xs[], int ys[], int vertexCount, Uint32 color);
	CPolygon(const CPolygon &copy);
	CPolygon &operator=(const CPolygon &copy);
	virtual ~CPolygon();

	virtual void draw(SDL_Surface *surface) const;

protected:
	int vertexCount;
	int *xs;
	int *ys;
	b2Vec2 *b2Vertices;

	virtual b2Body *createBody() const;

	void calculateXY();
	void calculateB2Vertices();
};