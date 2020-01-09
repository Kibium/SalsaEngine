#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "SDL_image/include/SDL_image.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface\n");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		auto width = DM.w;
		auto height = DM.h;

		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}
		if (BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}
		if (FSDESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width*.95, height*0.9, flags);

		if (window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface

			screen_surface = SDL_GetWindowSurface(window);
		}

		SDL_Surface* s = IMG_Load("../Source/Util/sauce.png");
		SDL_SetWindowIcon(window, s);

	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems\n");

	//Destroy window
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetFullscreen(const bool fullscreen) {
	fullscreen ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) : SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetResizable(const bool resizable) {

}

void ModuleWindow::SetBorderless(const bool borderless) {
	borderless ? SDL_SetWindowBordered(window, SDL_FALSE) : SDL_SetWindowBordered(window, SDL_TRUE);
}

void ModuleWindow::SetFullDesktop(const bool fulldesktop) {
	fulldesktop ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetWindowBrightness(const float brightness) {
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetWindowSize(const int width, const int height) {
	SDL_SetWindowSize(window, width, height);
}

