#pragma once

#include "Box2D/Box2D.h"
class CDrop;
class CPipe;

class CContactListener : public b2ContactListener
{
public:
	CContactListener(void);
	virtual ~CContactListener(void);

	// b2ContactListener functions
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

private:
	bool getDropAndPipe(b2Contact *contact, CDrop **drop, CPipe **pipe);
};

