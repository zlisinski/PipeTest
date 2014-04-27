#include <cmath>
#include <list>
#include <assert.h>
#include <Windows.h>
#include "SDL.h"
#include "Timer.h"
#include "Drop.h"

const Uint32 SCREEN_WIDTH = 1024;
const Uint32 SCREEN_HEIGHT = 768;
const Uint32 SCREEN_BPP = 32;
const Uint32 FRAME_RATE = 30;

CDrop createDrop(SDL_Surface *screen);
void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops);
void setPixel(SDL_Surface *screen, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b);


int main(int argc, char* args[])
{
	bool running = true;
	SDL_Surface *screen = NULL;
	SDL_Surface *image = NULL;
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

	SDL_Flip(screen);

	// Main loop
	while (running) {
		fps.start();

		drawScreen(screen, drops);

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
	SDL_Quit();

	return 0;
}

CDrop createDrop(SDL_Surface *screen)
{
	Uint32 radius = 5 + (rand() % 10);
	Uint32 r = rand() % 128;
	Uint32 g = 128 + (rand() % 128);
	Uint32 b = rand() % 128;
	Uint32 color = SDL_MapRGB(screen->format, r, g, b);
	Uint32 x = SCREEN_WIDTH - 50 + (rand() % 20);
	Uint32 y = 100 + (rand() % 20);

	return CDrop(radius, x, y, color);
}

void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops)
{
	const int margin = 30;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}

	// Clear screen each frame
	SDL_FillRect(screen, NULL, 0);

	// Add a new drop to the end each frame
	drops.push_back(createDrop(screen));

	std::list<CDrop>::iterator drop = drops.begin();
	while (drop != drops.end()) {
		// Move drop
		drop->move(drop->getX() - 2, drop->getY() + 2);

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