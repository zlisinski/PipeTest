#include "PointQueryCallback.h"


CPointQueryCallback::CPointQueryCallback(void)
{
}

CPointQueryCallback::~CPointQueryCallback(void)
{
}

bool CPointQueryCallback::ReportFixture(b2Fixture *fixture)
{
	void *userData = fixture->GetUserData();

	if (userData != NULL) {
		bodies.push_back(static_cast<CAbstractBody *>(userData));
	}

	return true;
}