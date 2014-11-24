#include <string>

#include "main.h"
#include "ContactListener.h"
#include "AbstractBody.h"
#include "Polygon.h"
#include "Drop.h"
#include "Pipe.h"

#include "Box2D/Box2D.h"

CContactListener::CContactListener(void) : b2ContactListener()
{
}

CContactListener::~CContactListener(void)
{
}

void CContactListener::BeginContact(b2Contact *contact)
{
	//debugPrint("BeginContact");
}

void CContactListener::EndContact(b2Contact *contact)
{
	//debugPrint("EndContact");
}

void CContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
	//debugPrint("PreSolve");
	
	CDrop *drop = NULL;
	CPipe *pipe = NULL;

	if (getDropAndPipe(contact, &drop, &pipe)) {
		//debugPrint("PreSolve Drop and Pipe");

		// Allow collisions based on the captureSide member of the pipe
		contact->SetEnabled(pipe->allowCollision(*drop));

		// Check if drop is completely inside pipe. Frame is not used yet.
		bool captured = pipe->captureDrop(*drop, 0);
		/*if (captured)
			debugPrint("Captured Drop");
		else
			debugPrint("Not Captured Drop");*/
	}
}

void CContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
{
	//debugPrint("PostSolve");
}

bool CContactListener::getDropAndPipe(b2Contact *contact, CDrop **outDrop, CPipe **outPipe)
{
	*outDrop = NULL;
	*outPipe = NULL;

	b2Fixture *fixtureA = contact->GetFixtureA();
	b2Fixture *fixtureB = contact->GetFixtureB();

	void *userDataA = fixtureA->GetUserData();
	void *userDataB = fixtureB->GetUserData();

	// Make sure we have userdata for both fixtures
	if (userDataA == NULL || userDataB == NULL) {
		//debugPrint("One or both Userdata is null: userDataA=%p, userDataB=%p", userDataA, userDataB);
		return false;
	}

	// The only UserData set is CAbstractBody*, so this should be safe
	CAbstractBody *bodyA = static_cast<CAbstractBody *>(userDataA);
	CAbstractBody *bodyB = static_cast<CAbstractBody *>(userDataB);

	CPipe *pipe = dynamic_cast<CPipe *>(bodyA);
	if (pipe == NULL)
		pipe = dynamic_cast<CPipe *>(bodyB);

	CDrop *drop = dynamic_cast<CDrop *>(bodyA);
	if (drop == NULL)
		drop = dynamic_cast<CDrop *>(bodyB);

	// Make sure we have a drop and a pipe
	if (pipe == NULL || drop == NULL) {
		//debugPrint("pipe or drop is null: pipe=%p, drop=%p", pipe, drop);
		return false;
	}

	*outPipe = pipe;
	*outDrop = drop;

	/*int pointCount = contact->GetManifold()->pointCount;
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	for (int i = 0; i < pointCount; i++) {
		debugPrint("x=%f, y=%f", worldManifold.points[i].x, worldManifold.points[i].y);
	}*/

	return true;
}