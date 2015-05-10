#pragma once

#include <cmath>
#include <stdexcept>

#include <SDL.h>
#include <Box2D/Box2D.h>

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
inline float pixelToMeter(Uint32 pixel)
{
	return pixel * 0.01f;
}

inline int meterToPixel(float meter)
{
	// VS2010 doesn't have round(), this is the way around that. Hopefully I'll upgrade to 2013, and cross-platform with Linux soon.
	if (meter < 0.0)
		return (int)ceil((meter * 100) - 0.5);
	return (int)floor((meter * 100) + 0.5);
	//return (int)round(meter * 100);
}

// Converts Y-up to Y-down coordinates and vice versa
// Example, with a screen height of 480px:
//          flipYAxis(100) = 379;
//          flipYAxis(379) = 100;
inline int flipYAxis(int y)
{
	return (SCREEN_HEIGHT - 1) - y;
}

inline Uint32 rgb(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(screen->format, r, g, b);
}

// Hopefully RVO is in effect... I haven't tested this, but I'm not worried about performance just yet.
inline b2Vec2 b2Vec2FromPixel(Uint32 x, Uint32 y)
{
	return b2Vec2(pixelToMeter(x), pixelToMeter(y));
}

/// Convert degrees to radians
inline float degreeToRadian(float degree)
{
	return degree * 3.1415f / 180;
}

/// Convert radians to degrees
inline float radianToDegree(float radian)
{
	return radian * 180 / 3.1415f;
}

void debugPrint(char *format, ...);
