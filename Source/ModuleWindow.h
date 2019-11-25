#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();
	void SetFullscreen(const bool);
	void SetResizable(const bool);
	void SetBorderless(const bool);
	void SetFullDesktop(const bool);
	void SetWindowBrightness(const float);
	void SetWindowSize(const int, const int);

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;
};

#endif // __ModuleWindow_H__