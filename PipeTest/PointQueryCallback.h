#pragma once
#include <list>
#include "AbstractBody.h"
#include "Box2D/Box2D.h"

class CPointQueryCallback :	public b2QueryCallback
{
public:
	CPointQueryCallback(void);
	virtual ~CPointQueryCallback(void);

	bool ReportFixture(b2Fixture *fixture);

	std::list<CAbstractBody *> bodies;
};

