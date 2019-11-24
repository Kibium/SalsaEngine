#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>



ModuleGUI::ModuleGUI()
{
}


ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init()
{
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	SDL_GetWindowMaximumSize(App->window->window, &max_w, &max_h);
	SDL_GetWindowMinimumSize(App->window->window, &min_w, &min_h);

	showScene = true;
	showAppWindow = true;
	showHelpWindow = false;
	showInspector = true;

	return true;
}

update_status ModuleGUI::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::Update()
{
	//ImGui::ShowDemoWindow();
	//Menu();
	MainMenu();
	if (showAppWindow)
		ShowDefWindow();
	if(showHelpWindow)
		ShowHelp();
	if (showScene)
		Scene();
	if (showInspector)
		GameObjecInfo();

	ShowConsole("Console");

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleGUI::EventManager(SDL_Event event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void ModuleGUI::ShowConsole(const char * title, bool * p_opened)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin(title, p_opened);
	if (ImGui::Button("Clear")) Clear();
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	Filter.Draw("Filter", -100.0f);
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (copy) ImGui::LogToClipboard();

	if (Filter.IsActive())
	{
		const char* buf_begin = logBuffer.begin();
		const char* line = buf_begin;
		for (int line_no = 0; line != NULL; line_no++)
		{
			const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
			if (Filter.PassFilter(line, line_end))
				ImGui::TextUnformatted(line, line_end);
			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}
	}
	else
	{
		ImGui::TextUnformatted(logBuffer.begin());
	}

	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}
void ModuleGUI::MainMenu() {

	if (ImGui::BeginMainMenuBar())
	{

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
			}
			if (ImGui::MenuItem("Open Scene"))
			{
			}
			if (ImGui::MenuItem("Save"))
			{
			}
			if (ImGui::MenuItem("New Project"))
			{
			}
			if (ImGui::MenuItem("Open Project"))
			{
			}
			if (ImGui::MenuItem("Save Project"))
			{
			}
			if (ImGui::MenuItem("Exit"))
			{
				SDL_Event quit_event;
				quit_event.type = SDL_QUIT;
				SDL_PushEvent(&quit_event);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game Object"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
			}
			if (ImGui::MenuItem("Effects"))
			{
			}
			if (ImGui::MenuItem("Lights"))
			{
			}
			if (ImGui::MenuItem("Audio"))
			{
			}
			if (ImGui::MenuItem("Video"))
			{
			}
			if (ImGui::MenuItem("UI"))
			{
			}
			if (ImGui::MenuItem("Camera"))
			{
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Component"))
		{
			if (ImGui::MenuItem("Mesh"))
			{
			}
			if (ImGui::MenuItem("Effects"))
			{
			}
			if (ImGui::MenuItem("Physics"))
			{
			}
			if (ImGui::MenuItem("Audio"))
			{
			}
			if (ImGui::MenuItem("Video"))
			{
			}
			if (ImGui::MenuItem("UI"))
			{
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem(("Application"), (const char*)0, &showAppWindow);
			ImGui::MenuItem(("Scene"), (const char*)0, &showScene);
			ImGui::MenuItem(("Inspector"), (const char*)0, &showInspector);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem(("Help"), (const char*)0, &showHelpWindow);

			ImGui::EndMenu();
		}

	}
	ImGui::EndMainMenuBar();

}
void ModuleGUI::Menu()
{
	bool* p_open = NULL;
	static bool fullscreen = false;
	static bool borderless = false;
	static bool resizable = false;
	static bool fsdesktop = false;

	static float display_brightness = SDL_GetWindowBrightness(App->window->window);
	static int screen_w = 0;
	static int screen_h = 0;
	SDL_GetWindowSize(App->window->window, &screen_w, &screen_h);

	if (!ImGui::Begin("SALSA Engine 0.1", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
	ImGui::Text("SALSA Engine with IMGUI. (%s)", IMGUI_VERSION);
	ImGui::Spacing();



	if (ImGui::CollapsingHeader("Help"))
	{
		ImGui::Text("PROGRAMMER GUIDE:");
		ImGui::BulletText("Please see the comments in imgui.cpp.");
		ImGui::BulletText("Please see the examples/ application.");
		ImGui::Separator();

		ImGui::Text("USER GUIDE:");
		ImGui::ShowUserGuide();
	}
	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::Text("Engine Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "0.1");
		ImGui::Separator();


		ImGui::SameLine();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		int fps = ImGui::GetIO().Framerate;
		//Get frames
		if (frames.size() > 120) //Max seconds to show
		{
			for (size_t i = 1; i < frames.size(); i++)
			{
				frames[i - 1] = frames[i];
			}
			frames[frames.size() - 1] = fps;
		}
		else
		{
			frames.push_back(fps);
		}
		char title[25];
		sprintf_s(title, 25, "Framerate: %d", fps);
		ImGui::PlotHistogram("##framerate", &frames[0], frames.size(), 0, title, 0.0f, 1000.0f, ImVec2(310, 100));
	}
	if (ImGui::CollapsingHeader("Window")) {

		if (ImGui::SliderFloat("Brightness", &display_brightness, 0, 1.00f))
			App->window->SetWindowBrightness(display_brightness);
		if (ImGui::SliderInt("Width", &screen_w, 640, 1024))
			App->window->SetWindowSize(screen_w, screen_h);
		if (ImGui::SliderInt("Height", &screen_h, 480, 720))
			App->window->SetWindowSize(screen_w, screen_h);
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			App->window->SetFullscreen(fullscreen);
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);
		if (ImGui::Checkbox("Borderless", &borderless))
			App->window->SetBorderless(borderless);
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &fsdesktop))
			App->window->SetFullDesktop(fsdesktop);

	}
	if (ImGui::CollapsingHeader("Hardware")) {
		SDL_version compiled;
		SDL_GetVersion(&compiled);
		ImGui::Text("SDL VERSION: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d. %d. %d ", compiled.major, compiled.minor, compiled.patch);
		ImGui::Text("CPU: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %d kb) ", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d GB", SDL_GetSystemRAM() / 1000);
		ImGui::Separator();
		char* vendor = (char*)glGetString(GL_VENDOR);
		char* card = (char*)glGetString(GL_RENDERER);
		char* ver = (char*)glGetString(GL_VERSION);
		ImGui::Text("GPU: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), card);
		ImGui::Text("Brand: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), vendor);
		ImGui::Text("Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), ver);
		GLint dedicatedMB = 0;
		GLint totalMB = 0;
		GLint currentMB = 0;
		//TODO STRING COMPARE IF NVIDIA OR AMD
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicatedMB);
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMB);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMB);
		ImGui::Text("VRAM:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)dedicatedMB / 1000);
		ImGui::Text("VRAM Budget:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)totalMB / 1000);
		ImGui::Text("VRAM Usage:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", ((float)totalMB / 1000) - ((float)currentMB / 1000));
		ImGui::Text("VRAM Available:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)currentMB / 1000);


	}
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::InputFloat3("Front", &App->camera->frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", &App->camera->frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Position", &App->camera->frustum.pos[0], 3, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();


		if (ImGui::SliderFloat("FOV", &App->camera->frustum.horizontalFov, 0, 2 * 3.14f))
		{
			App->camera->SetFOV(App->camera->frustum.horizontalFov);
		}

		if (ImGui::SliderFloat("Aspect Ratio", &App->camera->aspectRatio, 0, 10))
		{
			App->camera->SetAspectRatio(App->camera->aspectRatio);
		}

	}
	if (ImGui::CollapsingHeader("About..")) {

		ImGui::Text("JS ENGINE 0.1");
		ImGui::Text("This Engine was created as a project for the Master Degree 'Advanced Programming for AAA Video Games' made in the UPC from Barcelona");
		ImGui::Text("Authors:  "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Jordi Sauras Salas");
		ImGui::Text("Libraries Used: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "SLD2, GLEW, IMGUI");
		ImGui::Text("Liscense: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "TO DO");

	}
	//AFTER BLIND PUT VECTOR TO 0... EVERY GL CREATE, WILL NEED TO END AS A GL DELETE IN THE CLEANUP
	ImGui::End();
}
void ModuleGUI::Scene() {
	if (ImGui::Begin("Scene"))
	{
		isScene= ImGui::IsWindowHovered();
		float width = ImGui::GetWindowWidth();
		float height = ImGui::GetWindowHeight();
		App->camera->SetAspectRatio(width / height);
		App->renderer->DrawScene(width, height);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->frameTex,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + width,ImGui::GetCursorScreenPos().y + height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		
	}
	ImGui::End();
}

void ModuleGUI::GameObjecInfo() {
	if (ImGui::Begin("Inspector"))
	{
		isInspector = ImGui::IsWindowHovered();
		float width = ImGui::GetWindowWidth();
		float height = ImGui::GetWindowHeight();
		if (ImGui::CollapsingHeader("Transform")) {
			float3 position = (App->model->modelBox.maxPoint + App->model->modelBox.minPoint) / 2;
			float3 rotation = float3::zero;
			float3 scale = float3::one;
			ImGui::InputFloat3("Position", &position[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Rotation", &rotation[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Scale", &scale[0], 3, ImGuiInputTextFlags_ReadOnly);

		}
		if (ImGui::CollapsingHeader("Geometry")) {
			ImGui::InputInt("Mesh", &App->model->nmeshes , 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Triangles", &App->model->npolys, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Vertex", &App->model->nvertex, 0, 0, ImGuiInputTextFlags_ReadOnly);

		}
		if (ImGui::CollapsingHeader("Texture")) {

			if (ImGui::TreeNode("Texture's list"))
			{
				static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

				static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
				int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
				for (unsigned int i = 0; i < App->model->textures_loaded.size(); i++)
				{
					// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
					ImGuiTreeNodeFlags node_flags = base_flags;
					std::string str = App->model->textures_loaded[i].path;
					const char *path = str.c_str();
					const bool is_selected = (selection_mask & (1 << i)) != 0;
					if (is_selected)
						node_flags |= ImGuiTreeNodeFlags_Selected;
					if (i < App->model->textures_loaded.size())
					{
						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Texture %d", i);
						if (ImGui::IsItemClicked())
							node_clicked = i;
						if (node_open)
						{
							ImGui::Text("Path:");
							ImGui::SameLine();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), path);

							ImGui::Text("Texture %d size:", i);

							ImGui::SameLine();
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d x %d", App->model->textureWidth[i], App->model->textureHeight[i]);


							ImGui::Spacing();

							ImGui::Image((void*)(intptr_t)App->model->textures_loaded[i].id, ImVec2(width*0.5, width*0.5), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::TreePop();
						}
					}
				}
				if (node_clicked != -1)
				{
					// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
						selection_mask = (1 << node_clicked);           // Click to single-select
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
void ModuleGUI::ShowHelp() {
	bool* p_open = NULL;
	if (ImGui::Begin("HELP", p_open))
	{
		if (ImGui::CollapsingHeader("Help"))
		{
			ImGui::Text("PROGRAMMER GUIDE:");
			ImGui::BulletText("Please see the comments in imgui.cpp.");
			ImGui::BulletText("Please see the examples/ application.");
			ImGui::Separator();

			ImGui::Text("USER GUIDE:");
			ImGui::ShowUserGuide();
		}

		if (ImGui::CollapsingHeader("About..")) {

			ImGui::Text("SALSA ENGINE 0.1");
			ImGui::Text("This Engine was created as a project for the Master Degree 'Advanced Programming for AAA Video Games' made in the UPC from Barcelona");
			ImGui::Text("Authors:  "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Jordi Sauras Salas");
			ImGui::Text("Libraries Used: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "SLD2, GLEW, IMGUI");
			ImGui::Text("License: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "TO DO");

		}

	}
	ImGui::End();
}

void ModuleGUI::ShowDefWindow() {

	static bool fullscreen = false;
	static bool borderless = false;
	static bool resizable = false;
	static bool fsdesktop = false;

	static float display_brightness = SDL_GetWindowBrightness(App->window->window);
	static int screen_w = 0;
	static int screen_h = 0;
	SDL_GetWindowSize(App->window->window, &screen_w, &screen_h);
	bool* p_open = NULL;
	if (ImGui::Begin("Info Window", p_open))
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::Text("Engine Version: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "0.1");
			ImGui::Separator();

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			int fps = ImGui::GetIO().Framerate;
			//Get frames
			if (frames.size() > 120) //Max seconds to show
			{
				for (size_t i = 1; i < frames.size(); i++)
				{
					frames[i - 1] = frames[i];
				}
				frames[frames.size() - 1] = fps;
			}
			else
			{
				frames.push_back(fps);
			}
			char title[25];
			sprintf_s(title, 25, "Framerate: %d", fps);
			ImGui::PlotHistogram("##framerate", &frames[0], frames.size(), 0, title, 0.0f, 1000.0f, ImVec2(310, 100));
		}
		if (ImGui::CollapsingHeader("Window")) {

			if (ImGui::SliderFloat("Brightness", &display_brightness, 0, 1.00f))
				App->window->SetWindowBrightness(display_brightness);
			if (ImGui::SliderInt("Width", &screen_w, 640, 1024))
				App->window->SetWindowSize(screen_w, screen_h);
			if (ImGui::SliderInt("Height", &screen_h, 480, 720))
				App->window->SetWindowSize(screen_w, screen_h);
			if (ImGui::Checkbox("Fullscreen", &fullscreen))
				App->window->SetFullscreen(fullscreen);
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable))
				App->window->SetResizable(resizable);
			if (ImGui::Checkbox("Borderless", &borderless))
				App->window->SetBorderless(borderless);
			ImGui::SameLine();
			if (ImGui::Checkbox("Full Desktop", &fsdesktop))
				App->window->SetFullDesktop(fsdesktop);

		}
		if (ImGui::CollapsingHeader("Hardware")) {
			SDL_version compiled;
			SDL_GetVersion(&compiled);
			ImGui::Text("SDL VERSION: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d. %d. %d ", compiled.major, compiled.minor, compiled.patch);
			ImGui::Text("CPU: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %d kb) ", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
			ImGui::Text("System RAM: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d GB", SDL_GetSystemRAM() / 1000);
			ImGui::Separator();
			char* vendor = (char*)glGetString(GL_VENDOR);
			char* card = (char*)glGetString(GL_RENDERER);
			char* ver = (char*)glGetString(GL_VERSION);
			ImGui::Text("GPU: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), card);
			ImGui::Text("Brand: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), vendor);
			ImGui::Text("Version: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), ver);
			GLint dedicatedMB = 0;
			GLint totalMB = 0;
			GLint currentMB = 0;
			//TODO STRING COMPARE IF NVIDIA OR AMD
			glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicatedMB);
			glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMB);
			glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMB);
			ImGui::Text("VRAM:"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)dedicatedMB / 1000);
			ImGui::Text("VRAM Budget:"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)totalMB / 1000);
			ImGui::Text("VRAM Usage:"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", ((float)totalMB / 1000) - ((float)currentMB / 1000));
			ImGui::Text("VRAM Available:"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f MB", (float)currentMB / 1000);


		}
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::InputFloat3("Front", &App->camera->frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Up", &App->camera->frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Position", &App->camera->frustum.pos[0], 3, ImGuiInputTextFlags_ReadOnly);

			ImGui::Separator();


			if (ImGui::SliderFloat("FOV", &App->camera->frustum.horizontalFov, 0, 2 * 3.14f))
			{
				App->camera->SetFOV(App->camera->frustum.horizontalFov);
			}

			if (ImGui::SliderFloat("Aspect Ratio", &App->camera->aspectRatio, 0, 10))
			{
				App->camera->SetAspectRatio(App->camera->aspectRatio);
			}

		}

	}
	ImGui::End();
}