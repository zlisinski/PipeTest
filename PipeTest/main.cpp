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
#include "ContactListener.h"
#include "PointQueryCallback.h"

// SetLayoutFunc is a function pointer to a setLayout*() function
typedef void (*SetLayoutFunc) (std::list<CAbstractBody *> &bodies);

static void handleMouse(const SDL_Event &event);
static void drawScreen(SDL_Surface *screen, std::list<CDrop *> &drops, std::list<CAbstractBody *> &bodies);
static void drawGrid(SDL_Surface *screen);
static CAbstractBody *getBodyAtPoint(Uint32 x, Uint32 y);
static void setLayout1(std::list<CAbstractBody *> &bodies);
static void setLayout2(std::list<CAbstractBody *> &bodies);
static void setLayout3(std::list<CAbstractBody *> &bodies);
static void setLayout4(std::list<CAbstractBody *> &bodies);
static void cleanup(std::list<CDrop *> &drops, std::list<CAbstractBody *> &bodies);

// Globals for convenience, so they don't have to be passed in to every function
SDL_Surface *screen = NULL;
b2World *world = NULL;
bool gridVisible = true;
int mouseX = 0;
int mouseY = 0;
bool mouseDownL = false;
bool mouseDownR = false;
CAbstractBody *draggingBody = NULL;
CPolygon *draggingPolygon = NULL;
int draggingVertexIndex = -1;
b2Vec2 draggingDelta;

int main(int argc, char* args[])
{
	bool running = true;
	int frame = 0;
	CTimer fps;
	std::list<CDrop *> drops;
	std::list<CAbstractBody *> bodies; // I know pointers aren't the best here, I'll change it later
	std::list<CPipe> pipes;
	int curLayout = 2;
	SetLayoutFunc layoutFuncs[] = {setLayout1, setLayout2, setLayout3, setLayout4};
	const int LAYOUT_COUNT = sizeof(layoutFuncs) / sizeof(layoutFuncs[0]);
	CContactListener contactListener;

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
	world->SetContactListener(&contactListener);

	// Setup the walls and polygons
	layoutFuncs[curLayout](bodies);

	// Main loop
	while (running) {
		fps.start();

		world->Step(TIME_STEP, VELOCITY_ITER, POSITION_ITER);

		drawScreen(screen, drops, bodies);

		SDL_Event event;
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

			handleMouse(event);
		}

		char title[256];
		sprintf_s(title, "Pipe Test - Press 'z' to change layout, Press 'x' to toggle grid. Drop count = %d. Mouse=%d,%d,%s%s",
		          drops.size(), mouseX, mouseY, mouseDownL ? "L" : "", mouseDownR ? "R" : "");
		SDL_WM_SetCaption(title, NULL);

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

static void handleMouse(const SDL_Event &event)
{
	if (event.type == SDL_MOUSEMOTION) {
		mouseX = event.motion.x;
		mouseY = flipYAxis(event.motion.y);
		if (draggingBody != NULL) {
			b2Vec2 point = b2Vec2FromPixel(mouseX, mouseY);
			b2Vec2 newPos = point + draggingDelta;

			if (draggingPolygon != NULL && draggingVertexIndex >= 0) 
				draggingPolygon->moveVertex(draggingVertexIndex, meterToPixel(newPos.x), meterToPixel(newPos.y));
			else
				draggingBody->move(meterToPixel(newPos.x), meterToPixel(newPos.y));
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			mouseDownL = true;

			// Get body at click point
			draggingBody = getBodyAtPoint(mouseX, mouseY);

			// If we have a body, and it's a CPolygon, check if the click point is near a vertex
			draggingPolygon = dynamic_cast<CPolygon *>(draggingBody);
			if (draggingPolygon != NULL) {
				draggingVertexIndex = draggingPolygon->getNearestVertextIndex(mouseX, mouseY);
				//draggingPolygon->getN
				debugPrint("draggingVertex=%d", draggingVertexIndex);
			}

			if (draggingBody == NULL) {
				debugPrint("Click=NULL");
			}
			else {
				debugPrint("Click=%d,%d Body=%d,%d", mouseX, mouseY, draggingBody->getX(), draggingBody->getY());
				b2Vec2 point = b2Vec2FromPixel(mouseX, mouseY);
				b2Vec2 bodyPoint;

				if (draggingPolygon != NULL && draggingVertexIndex >= 0)
					bodyPoint = draggingPolygon->getVertex(draggingVertexIndex);
				else
					bodyPoint = b2Vec2FromPixel(draggingBody->getX(), draggingBody->getY());

				// Calculate delta between where we clicked and what we are acting on, so that later we can move the
				// body or vertex by the amount that the mouse pointer moved, not to where the mouse pointer is.
				draggingDelta = bodyPoint - point;
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT) {
			mouseDownR = true;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			mouseDownL = false;
			if (draggingBody != NULL) {
				b2Vec2 point = b2Vec2FromPixel(mouseX, mouseY);
				b2Vec2 newPos = point + draggingDelta;

				if (draggingPolygon != NULL && draggingVertexIndex >= 0) 
					draggingPolygon->moveVertex(draggingVertexIndex, meterToPixel(newPos.x), meterToPixel(newPos.y));
				else
					draggingBody->move(meterToPixel(newPos.x), meterToPixel(newPos.y));

				// Clear dragging variables
				draggingBody = NULL;
				draggingPolygon = NULL;
				draggingVertexIndex =-1;
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT) {
			mouseDownR = false;
		}
	}
}

static void drawScreen(SDL_Surface *screen, std::list<CDrop *> &drops, std::list<CAbstractBody *> &bodies)
{
	static unsigned int frame = 0;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}

	// Clear screen each frame
	SDL_FillRect(screen, NULL, 0);

	// Draw grid
	if (gridVisible)
		drawGrid(screen);

	// Add a new drop each frame.
	drops.push_back(new CDrop());
	drops.push_back(new CDrop());
	drops.push_back(new CDrop());

	std::list<CDrop *>::iterator ppDrop = drops.begin();
	while (ppDrop != drops.end()) {
		CDrop *pDrop = *ppDrop;

		// Remove captured drops
		if (pDrop->getCaptured()) {
			drops.erase(ppDrop++);
			continue;
		}

		// Move drop
		pDrop->update();

		if ((pDrop->getMinX() > 0) &&
		    (pDrop->getMaxX() < SCREEN_WIDTH) &&
		    (pDrop->getMinY() > 0) &&
		    (pDrop->getMaxY() < SCREEN_HEIGHT)) {
				// Draw drop if it is still on the screen
				pDrop->draw(screen, frame);
				++ppDrop;
		}
		else {
			// Remove it if it hits the edge
			drops.erase(ppDrop++);
			delete pDrop;
		}
	}

	// Draw bodies
	for (std::list<CAbstractBody *>::iterator pBody = bodies.begin(); pBody != bodies.end(); ++pBody) {
		(*pBody)->draw(screen, frame);

		// If the body is a pipe, check if there are any drops to be emitted
		CPipe *pipe = dynamic_cast<CPipe *>(*pBody);
		if (pipe != NULL) {
			std::list<CDrop *> newDrops = pipe->emitDrops(frame);
			if (!newDrops.empty()) {
				drops.splice(drops.end(), newDrops);
			}
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_Flip(screen);

	// This is safe. It overflows after two years at 60fps
	frame++;
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

static CAbstractBody *getBodyAtPoint(Uint32 x, Uint32 y)
{
	b2Vec2 point = b2Vec2FromPixel(x, y);
	b2AABB aabb;
	aabb.lowerBound = aabb.upperBound = point;

	CPointQueryCallback callback;

	world->QueryAABB(&callback, aabb);

	for (std::list<CAbstractBody *>::iterator pBody = callback.bodies.begin(); pBody != callback.bodies.end(); ++pBody) {
		// According to a stackoverflow post, QueryAABB can return bodies whose AABB contains the point, not just 
		// actual fixtures containing the point. That doesn't seem to be the case, but check with the body anyway.
		if ((*pBody)->testPoint(x, y)) {
			return *pBody;
		}
	}

	return NULL;
}

static void setLayout1(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 0, 1024, 10, rgb(150, 75, 0)));
	bodies.push_back(new CPolygon(650, 350, 100, 50, rgb(0, 0, 128)));

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
	bodies.push_back(new CPolygon(0, 0, 1024, 10, rgb(150, 75, 0)));

	// Add pentagon
	int xs[] = {550, 650, 600, 500, 450};
	int ys[] = {300, 200, 100, 100, 200};
	bodies.push_back(new CPolygon(xs, ys, 5));
	
	// Add triangle
	int xs2[] = {700, 750, 725};
	int ys2[] = {150, 150, 250};
	bodies.push_back(new CPolygon(xs2, ys2, 3));
}

static void setLayout3(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 0, 1024, 10, rgb(150, 75, 0)));

	// Add pipes
	bodies.push_back(new CPipe(700, 400, 100, 100, CPipe::Top, CPipe::Left));
	bodies.push_back(new CPipe(200, 300, 200, 100, CPipe::Top, CPipe::Left));
}

static void setLayout4(std::list<CAbstractBody *> &bodies)
{
	// Add bodies
	bodies.push_back(new CPolygon(0, 0, 1024, 10, rgb(150, 75, 0)));

	// Add pipes
	bodies.push_back(new CPipe(700, 400, 100, 100, CPipe::Top, CPipe::Left));
	bodies.push_back(new CPipe(400, 500, 100, 100, CPipe::Right, CPipe::Left));
}

static void cleanup(std::list<CDrop *> &drops, std::list<CAbstractBody *> &bodies)
{
	for (std::list<CDrop *>::iterator drop = drops.begin(); drop != drops.end(); ++drop) {
		delete *drop;
	}
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