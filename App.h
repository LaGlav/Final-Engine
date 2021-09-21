#include <SDL.h>


#include "Defs.h"

typedef struct App {

	SDL_Window* win;
	SDL_Surface* win_surf;
	float win_scale;
	int win_w, win_h;

	SDL_Renderer* ren;

	bool state;

	SDL_Texture* scr;

	int FPS;

	Uint64 time_ms, frame_ms, update_ms, render_ms;
	double dt;
}App;

extern App app;

void App_init();
void App_update();
void App_render();
void App_close();