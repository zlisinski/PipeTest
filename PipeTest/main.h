#pragma once

#include <cmath>

#include <SDL.h>

struct SDL_Surface;
class b2World;

extern SDL_Surface *screen;
extern b2World *world;

const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;
const unsigned int SCREEN_BPP = 32;
const unsigned int FRAME_RATE = 30;
const float TIME_STEP = 1.0f / 60.0f;
const int VELOCITY_ITER = 8;
const int POSITION_ITER = 3;

const short DROP_CATEGORY = 1;
const short WALL_CATEGORY = 2; // Walls and Polygons
const short PIPE_CATEGORY = 4;

const unsigned short DROP_MASK = 0xFFFE; // Drops don't collide with other drops 1111 1111 1111 1110
const unsigned short WALL_MASK = 0xFFFF; // Walls collide with everything
const unsigned short PIPE_MASK = 0xFFFF; // Pipes collide with everything

// 1px == 1cm
inline float pixelToMeter(float pixel)
{
	return pixel * 0.01f;
}

inline int meterToPixel(float meter)
{
	return (int)floor(meter * 100);
}

// Converts Y-up to Y-down coordinates and vice versa
// Example, with a screen height of 480px:
//          flipYAxis(100) = 379;
//          flipYAxis(379) = 100;
inline int flipYAxis(int y)
{
	return (-1 * y) + (SCREEN_HEIGHT - 1);
}

inline Uint32 rgb(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(screen->format, r, g, b);
}

void debugPrint(char *format, ...);
