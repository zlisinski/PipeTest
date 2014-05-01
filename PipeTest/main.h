#pragma once

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


// 1px == 1cm
inline float pixelToMeter(float pixel)
{
	return pixel * 0.01f;
}

inline float meterToPixel(float meter)
{
	return meter * 100;
}

inline int flipYAxis(int y)
{
	return (-1 * y) + (SCREEN_HEIGHT - 1);
}

void debugPrint(char *format, ...);
