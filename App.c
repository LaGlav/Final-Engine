#include <stdio.h>

#include "App.h"

#include "SDL_image.h"


//App instance
static App app;

void App_SetColor(r, g, b, a)
{
	SDL_SetRenderDrawColor(app.ren, r, g, b, a);
}

void App_blit(SDL_Texture * tex, SDL_Rect* dst)
{
	
}

SDL_Surface* App_CreateSurface(int w, int h)
{
	SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	return surface;
}

SDL_Texture* App_CreateTexture(w, h) {
	return SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
}

void App_init()
{
	printf("App initalizing...\n");

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Could not initalize SDL.\n");
		printf("SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (IMG_Init(IMG_INIT_JPG) == false) {
		printf("Error initalizing image system...\n");
		exit(1);
	}

	IMG_Init(IMG_INIT_PNG);

	app.state = APP_RUNNING;

	app.win = NULL;
	app.win_scale = DEFAULT_WIN_SCALE;
	app.win_w = SCR_W * app.win_scale;
	app.win_h = SCR_H * app.win_scale;

	app.win = SDL_CreateWindow("SDL Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app.win_w, app.win_h, SDL_WINDOW_SHOWN);
	if (app.win == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		App_close();
	}

	app.win_surf = SDL_GetWindowSurface(app.win);
	app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_ACCELERATED);
	
	app.scr = App_CreateTexture(SCR_W, SCR_H);
	if (app.scr == NULL) {
		printf("Something went wrong creating the game screen...\n");
		App_close();
	}

	//init vars
	app.FPS = DEFAULT_FPS;

	app.frame_ms = 0;
	app.render_ms = 0;
	app.update_ms = 0;

}

//player data
SDL_Rect p = {0, 0, 16, 16};
double x = 10, y = 100, vel_x = 0.0, vel_y = 0.0;
const int speed_x = 1500;
static bool on_floor;

void P_Update()
{
	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_A]) {
		if (on_floor)
			vel_x -= 4000 * app.dt;
		else
			vel_x -= 800 * app.dt;
	}
	else if (state[SDL_SCANCODE_D]) {
		if (on_floor)
			vel_x += 4000 * app.dt;
		else
			vel_x += 800 * app.dt;
	}

	//Gravity
	vel_y += 2000 * app.dt;

	//Friction
	if (!on_floor) //Off ground
		vel_x -= 2.0 * vel_x * app.dt;
	else // On ground
		vel_x -= 14 * vel_x * app.dt;

	if ((vel_x) > speed_x)
		vel_x = speed_x;
	if ((vel_x) < -speed_x)
		vel_x = -speed_x;

	y += vel_y * app.dt;
	x += vel_x * app.dt;


	if (x > (double)SCR_W - p.w)
	{
		x = (double)SCR_W - p.w;
		vel_x *= -1;
	}
	else if (x < 0)
	{
		x = 0.0;
		vel_x *= -1;
	}

	//Cheking if the player is grounded
	if (y < SCR_H - 100.0 - p.h) { //Off ground
		on_floor = false;
		//printf("off floor\n");
	}
	else // On ground
	{
		y = SCR_H - 100.0 - p.h;
		vel_y = 0;
		on_floor = true;
	}

	p.x = (int)round(x);
	p.y = (int)round(y);
}


void App_update()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0) {
		//User requests quit
		if (e.type == SDL_QUIT) {
			App_close();
		}
		switch (e.type)
		{
		case SDL_KEYDOWN:
			//printf("key press:\n");
			switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				if (on_floor == true)
					vel_y = -500;
				break;
			}
			break;
		}
	}

	P_Update();

}


void App_render()
{
	SDL_SetRenderTarget(app.ren, app.scr);

	//SDL Rects representing window and screen dimensions
	SDL_Rect scr_dim = { 0, 0, SCR_W, SCR_H };
	SDL_Rect win_dim = { 0, 0, app.win_w, app.win_h };

	//Drawing here
	// 
	//Background
	App_SetColor(100, 50, 220, 255);
	SDL_RenderClear(app.ren);

	//Floor
	App_SetColor(50, 0, 150, 255);
	SDL_RenderFillRect(app.ren, &(SDL_Rect){0, SCR_H - 100, SCR_W, 100 });

	//Player
	App_SetColor(200, 200, 0, 255);
	SDL_RenderFillRect(app.ren, &p);
	
	//Scale and draw the screen surface to fill the window
	if (SDL_SetRenderTarget(app.ren, NULL) == true) {
		printf("SDL_ERROR: %s", SDL_GetError());
	}

	SDL_RenderCopy(app.ren, app.scr, NULL, NULL);


	//Flip the display buffer
	SDL_RenderPresent(app.ren);
}


void App_close()
{
	printf("\nTerminating app...\n");
	SDL_DestroyWindow(app.win);
	SDL_DestroyRenderer(app.ren);
	SDL_Quit();
	exit(0);
	app.state = APP_CLOSED;
}

int main()
{
	printf("Final engine\n");
	
	//Creating an app. instance
	App_init(app);

	//Main Loop
	while (true) {
		Uint64 start = SDL_GetPerformanceCounter();

		App_update();
		App_render();

		//FPS console display
		static double interval = 0.0;

		if (interval > 3) {
			printf("FPS: %d\n", (int)round(1 / app.dt));
			interval = 3 - interval;
		}

		Uint64 end = SDL_GetPerformanceCounter();

		app.dt = (end - start) / (float)SDL_GetPerformanceFrequency();

		if ( app.dt < 1.0/app.FPS)
		{
			SDL_Delay((1.0/app.FPS-app.dt) * 1000);
			app.dt = 1.0 / app.FPS;
		}
		interval += app.dt;
	}

	return 0;
}