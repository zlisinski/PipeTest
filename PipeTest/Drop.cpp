#include <cmath>
#include "SDL.h"
#include "SDL_draw.h"
#include "Drop.h"


CDrop::CDrop() : radius(0), x(0), y(0), color(0)
{
}

CDrop::CDrop(Uint32 radius, Uint32 x, Uint32 y, Uint32 color) : radius(radius), x(x), y(y), color(color)
{
}

CDrop::CDrop(const CDrop &copy) : radius(copy.radius), x(copy.x), y(copy.y), color(copy.color)
{
}

CDrop &CDrop::operator=(const CDrop &copy)
{
	if (this != &copy) {
		this->radius = copy.radius;
		this->x = copy.x;
		this->y = copy.y;
		this->color = copy.color;
	}

	return *this;
}

CDrop::~CDrop()
{
}

void CDrop::move(Uint32 x, Uint32 y)
{
	this->x = x;
	this->y = y;
}

void CDrop::draw(SDL_Surface *surface)
{
	Draw_FillCircle(surface, this->x, this->y, this->radius, this->color);
}
