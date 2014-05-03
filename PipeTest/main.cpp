#include <list>
#include <assert.h>
#include <cstdio>
#include <Windows.h>

#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Timer.h"
#include "Drop.h"
#include "Wall.h"

void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, std::list<CWall> &walls);

SDL_Surface *screen = NULL;
b2World *world = NULL;

int main(int argc, char* args[])
{
	bool running = true;
	SDL_Event event;
	int frame = 0;
	CTimer fps;
	std::list<CDrop> drops;
	std::list<CWall> walls;

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
	gravity.Set(0.0f, -10.0f);
	world = new b2World(gravity);

	// Add walls
	walls.push_back(CWall(0, 10, 1024, 10, SDL_MapRGB(screen->format, 150, 75, 0)));
	walls.push_back(CWall(650, 400, 100, 300, SDL_MapRGB(screen->format, 0, 0, 128)));

	// Main loop
	while (running) {
		fps.start();

		char title[256];
		sprintf_s(title, "Pipe Test - Drop count = %d", drops.size());
		SDL_WM_SetCaption(title, NULL);

		world->Step(TIME_STEP, VELOCITY_ITER, POSITION_ITER);

		drawScreen(screen, drops, walls);

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
	walls.clear();
	delete world;
	world = NULL;
	SDL_Quit();

	return 0;
}

void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, std::list<CWall> &walls)
{
	const int margin = 30;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}
	
	// Clear screen each frame
	SDL_FillRect(screen, NULL, 0);

	// Draw grid
	for (int x = 0; x < SCREEN_WIDTH; x += 50) {
		Draw_VLine(screen, x, 0, SCREEN_HEIGHT - 1, SDL_MapRGB(screen->format, 255, 255, 255));
	}
	for (int y = 0; y < SCREEN_HEIGHT; y += 50) {
		Draw_HLine(screen, 0, y, SCREEN_WIDTH - 1, SDL_MapRGB(screen->format, 255, 255, 255));
	}

	// Draw walls
	for (std::list<CWall>::iterator wall = walls.begin(); wall != walls.end(); ++wall) {
		wall->draw(screen);
	}

	// Add a new drop to the end each frame
	drops.push_back(CDrop());

	std::list<CDrop>::iterator drop = drops.begin();
	while (drop != drops.end()) {
		// Move drop
		drop->update();

		if (((drop->getX() - drop->getRadius()) > 0) &&
		    ((drop->getX() + drop->getRadius()) < SCREEN_WIDTH) &&
		    ((drop->getY() - drop->getRadius()) > 0) &&
		    ((drop->getY() + drop->getRadius()) < SCREEN_HEIGHT)) {
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