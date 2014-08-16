#include <list>
#include <assert.h>
#include <cstdio>
#include <Windows.h>
#include <time.h>

#include "SDL.h"
#include "SDL_draw.h"
#include "Box2D/Box2D.h"

#include "main.h"
#include "Timer.h"
#include "Drop.h"
#include "Polygon.h"
#include "Pipe.h"

// SetLayoutFunc is a function pointer to a setLayout*() function
typedef void (*SetLayoutFunc) (std::list<CAbstractBody *> &bodies);

static void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, std::list<CAbstractBody *> &bodies);
static void drawGrid(SDL_Surface *screen);
static void setLayout1(std::list<CAbstractBody *> &bodies);
static void setLayout2(std::list<CAbstractBody *> &bodies);
static void setLayout3(std::list<CAbstractBody *> &bodies);
static void cleanup(std::list<CDrop> &drops, std::list<CAbstractBody *> &bodies);

// Globals for convenience, so they don't have to be passed in to every function
SDL_Surface *screen = NULL;
b2World *world = NULL;
bool gridVisible = true;

const int LAYOUT_COUNT = 3;

int main(int argc, char* args[])
{
	bool running = true;
	SDL_Event event;
	int frame = 0;
	CTimer fps;
	std::list<CDrop> drops;
	std::list<CAbstractBody *> bodies; // I know pointers aren't the best here, I'll change it later
	int curLayout = 0;
	SetLayoutFunc layoutFuncs[LAYOUT_COUNT] = {setLayout1, setLayout2, setLayout3};

	srand((unsigned int)time(NULL));

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

	// Setup the walls and polygons
	layoutFuncs[curLayout](bodies);

	// Main loop
	while (running) {
		fps.start();

		char title[256];
		sprintf_s(title, "Pipe Test - Press 'z' to change layout, Press 'x' to toggle grid. Drop count = %d", drops.size());
		SDL_WM_SetCaption(title, NULL);

		world->Step(TIME_STEP, VELOCITY_ITER, POSITION_ITER);

		drawScreen(screen, drops, bodies);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_KEYDOWN) {
				SDLKey keyPressed = event.key.keysym.sym;
				if (keyPressed == SDLK_q) {
					// Quit running
					running = false;
					break;
				}
				if (keyPressed == SDLK_x) {
					// Toggle gridVisible
					gridVisible = !gridVisible;
				}
				if (keyPressed == SDLK_z) {
					// Change layout
					curLayout = (curLayout + 1) % LAYOUT_COUNT;
					cleanup(drops, bodies);
					layoutFuncs[curLayout](bodies);
				}
			}
		}

		// Lock the frame rate
		if (fps.get_ticks() < 1000 / FRAME_RATE) {
			SDL_Delay((1000 / FRAME_RATE) - fps.get_ticks());
		}
	}

	// Cleanup
	cleanup(drops, bodies);
	delete world;
	world = NULL;
	SDL_Quit();

	return 0;
}

static void drawScreen(SDL_Surface *screen, std::list<CDrop> &drops, std::list<CAbstractBody *> &bodies)
{
	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}
	
	// Clear screen each frame
	SDL_FillRect(screen, NULL, 0);

	// Draw grid
	if (gridVisible)
		drawGrid(screen);

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

	// Draw bodies
	for (std::list<CAbstractBody *>::iterator body = bodies.begin(); body != bodies.end(); ++body) {
		(*body)->draw(screen);
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_Flip(screen);
}

static void drawGrid(SDL_Surface *screen)
{
	int color;

	for (int x = 0; x < SCREEN_WIDTH; x += 10) {
		if (x % 100 == 0)
			color = rgb(224, 224, 224);
		else if (x % 50 == 0)
			color = rgb(128, 128, 128);
		else
			color = rgb(32, 32, 32);

		Draw_VLine(screen, x, 0, SCREEN_HEIGHT - 1, color);
	}

	for (int y = 0; y < SCREEN_HEIGHT; y += 10) {
		if (y % 100 == 0)
			color = rgb(224, 224, 224);
		else if (y % 50 == 0)
			color = rgb(128, 128, 128);
		else
			color = rgb(32, 32, 32);

		// Flip Y axis because SDL is Y-down while the rest of the code is Y-up
		int newY = flipYAxis(y);

		Draw_HLine(screen, 0, newY, SCREEN_WIDTH - 1, color);
	}
}

static void setLayout1(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 10, 1024, 10, rgb(150, 75, 0)));
	bodies.push_back(new CPolygon(650, 400, 100, 50, rgb(0, 0, 128)));

	int xs[] = {400, 450, 400};
	int ys[] = {100, 100, 150};
	bodies.push_back(new CPolygon(xs, ys, 3));

	int xs2[] = {800, 850, 875};
	int ys2[] = {10, 10, 35};
	bodies.push_back(new CPolygon(xs2, ys2, 3));
}

static void setLayout2(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 10, 1024, 10, rgb(150, 75, 0)));

	// Add pentagon
	int xs[] = {550, 650, 600, 500, 450};
	int ys[] = {300, 200, 100, 100, 200};
	bodies.push_back(new CPolygon(xs, ys, 5));
}

static void setLayout3(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 10, 1024, 10, rgb(150, 75, 0)));

	bodies.push_back(new CPipe(600, 300, 100, 100));
}

static void cleanup(std::list<CDrop> &drops, std::list<CAbstractBody *> &bodies)
{
	drops.clear();

	for (std::list<CAbstractBody *>::iterator body = bodies.begin(); body != bodies.end(); ++body) {
		delete *body;
	}
	bodies.clear();
}

void debugPrint(char *format, ...)
{
	char str[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(str, format, args);
	OutputDebugString(str);
	OutputDebugString("\n");
	va_end(args);
}