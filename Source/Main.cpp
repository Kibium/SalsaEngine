#include <stdlib.h>
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGUI.h"
#include "Globals.h"

#include "SDL.h"
#pragma comment( lib, "Libraries/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Libraries/SDL/libx86/SDL2main.lib" )

#include "optick/optick.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	LOG(buffer);

	while (state != MAIN_EXIT)
	{
		OPTICK_FRAME("MainThread");

		switch (state)
		{
		case MAIN_CREATION:

			LOG("\nApplication Creation --------------\n");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("Application Init --------------\n");
			if (App->Init() == false)
			{
				LOG("Application Init exits with error -----\n");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("Application Update --------------\n");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with error -----\n");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("Application CleanUp --------------\n");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----\n");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}

	}

	delete App;
	LOG("Bye :)\n");
	return main_return;
}
