#pragma once
class CDrop
{
public:
	CDrop();
	CDrop(Uint32 radius, Uint32 x, Uint32 y, Uint32 color);
	CDrop(const CDrop &copy);
	CDrop &operator=(const CDrop &copy);
	virtual ~CDrop();

	void move(Uint32 x, Uint32 y);
	void draw(SDL_Surface *surface);

	Uint32 getRadius() {return radius;}
	Uint32 getX() {return x;}
	Uint32 getY() {return y;}
	Uint32 getColor() {return color;}

private:
	Uint32 radius;
	Uint32 x;
	Uint32 y;
	Uint32 color;
};

