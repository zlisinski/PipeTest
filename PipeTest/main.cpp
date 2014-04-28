#include <cmath>
#include <list>
#include <assert.h>
#include <cstdio>
#include <Windows.h>

#include "SDL.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Timer.h"
#include "Drop.h"
#include "Wall.h"

void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, CWall *wall);
void setPixel(SDL_Surface *screen, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b);

SDL_Surface *screen = NULL;
b2World *world = NULL;

int main(int argc, char* args[])
{
	bool running = true;
	SDL_Event event;
	int frame = 0;
	CTimer fps;
	std::list<CDrop> drops;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	// Set the window caption
	SDL_WM_SetCaption("Pipe Test", NULL);

	// Set the screen size
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL) {
		SDL_Quit();
		return 1;
	}

	// Initialize box2d world
	b2Vec2 gravity;
	gravity.Set(0.0f, 10.0f);
	world = new b2World(gravity);

	CWall *wall = new CWall(0, 757, 1024, 9);

	// Main loop
	while (running) {
		fps.start();

		char title[256];
		sprintf_s(title, "Pipe Test - Drop count = %d", drops.size());
		SDL_WM_SetCaption(title, NULL);

		world->Step(TIME_STEP, VELOCITY_ITER, POSITION_ITER);

		drawScreen(screen, drops, wall);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_KEYDOWN) {
				SDLKey keyPressed = event.key.keysym.sym;
				if (keyPressed == SDLK_q) {
					running = false;
					break;
				}
			}
		}

		// Lock the frame rate
		if (fps.get_ticks() < 1000 / FRAME_RATE) {
			SDL_Delay((1000 / FRAME_RATE) - fps.get_ticks());
		}
	}

	// Cleanup
	drops.clear();
	delete wall;
	delete world;
	world = NULL;
	SDL_Quit();

	return 0;
}

void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, CWall *wall)
{
	const int margin = 30;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}

	// Clear screen each frame
	SDL_FillRect(screen, NULL, 0);

	wall->draw(screen);

	// Add a new drop to the end each frame
	drops.push_back(CDrop());

	std::list<CDrop>::iterator drop = drops.begin();
	while (drop != drops.end()) {
		// Move drop
		drop->update();

		if (drop->getX() + drop->getRadius() < SCREEN_WIDTH &&
			drop->getY() + drop->getRadius() < SCREEN_HEIGHT) {
				// Draw drop if it is still on the screen
				drop->draw(screen);
				++drop;
		}
		else {
			// Remove it if it hits the edge
			drops.erase(drop++);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_Flip(screen);
}

void debugPrint(char *format, ...)
{
	char str[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(str, format, args);
	OutputDebugString(str);
	va_end(args);
}