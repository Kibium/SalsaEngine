#include "ModuleGUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleModelLoader.h"
#include "ModuleMSTimer.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "IconsMaterialDesignIcons.h"
#include "IconsFontAwesome5.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "ModuleScene.h"
#include "ComponentCamera.h"
#include "optick/optick.h"

ModuleGUI::ModuleGUI() {
}


ModuleGUI::~ModuleGUI() {
}


void ModuleGUI::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool ModuleGUI::Init() {

	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigWindowsMoveFromTitleBarOnly = TRUE;

	ImGui::JordiStyle();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	SDL_GetWindowMaximumSize(App->window->window, &max_w, &max_h);
	SDL_GetWindowMinimumSize(App->window->window, &min_w, &min_h);

	io.Fonts->AddFontDefault();

	// merge in icons
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF("Fonts/" FONT_ICON_FILE_NAME_FAS, 14.0f, &icons_config, icons_ranges);

	return true;
}

update_status ModuleGUI::PreUpdate() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::Update() {

	OPTICK_CATEGORY("UpdateUI", Optick::Category::UI);

	MainMenu();

	if (showAppWindow)
		ShowDefWindow();

	if (showHelpWindow)
		ShowHelp();

	/*if (showGame)
		Game();

	if (showScene)
		Scene();*/
	if (showScene)
		MainWindow();
	if (true)
		GameObjecInfo();

	if (showAboutWindow)
		ShowAbout();

	ShowTimeButtons();

	if (showHierarchy)
		App->scene->DrawHierarchy(&showHierarchy);

	if (showInspector)
		App->scene->DrawInspector(&showInspector);

	ShowConsole(ICON_FA_TERMINAL " Console");

	ImGui::ShowDemoWindow();

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::PostUpdate() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleGUI::EventManager(SDL_Event event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void ModuleGUI::ShowConsole(const char * title, bool * p_opened) {
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin(title, p_opened);
	if (ImGui::Button("Clear")) Clear();
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	if (Filter.IsActive()) {
		const char* buf_begin = logBuffer.begin();
		const char* line = buf_begin;
		for (int line_no = 0; line != NULL; line_no++) {
			const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
			if (Filter.PassFilter(line, line_end))
				ImGui::TextUnformatted(line, line_end);
			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}
	}
	else {
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

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene")) {
			}
			if (ImGui::MenuItem("Open Scene")) {
			}
			if (ImGui::MenuItem("Save")) {
			}
			if (ImGui::MenuItem("New Project")) {
			}
			if (ImGui::MenuItem("Open Project")) {
			}
			if (ImGui::MenuItem("Save Project")) {
			}
			if (ImGui::MenuItem("Exit")) {
				SDL_Event quit_event;
				quit_event.type = SDL_QUIT;
				SDL_PushEvent(&quit_event);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game Object")) {
			if (ImGui::MenuItem("Create Empty")) {
			}

			if (ImGui::MenuItem("Create Sphere"))
			{
				App->model->CreateSphere("sphere0", math::float3(10.0f, 0.0f, 0.0f), math::Quat::identity, 0.5f, 30, 30, App->model->color);
				App->model->materials.back().k_specular = 0.9f;
				App->model->materials.back().shininess = 64.0f;
				App->model->materials.back().k_specular = 0.6f;
				App->model->materials.back().k_diffuse = 0.5f;
				App->model->materials.back().k_ambient = 1.0f;
			}
			if (ImGui::MenuItem("Create Torus"))
			{
				App->model->CreateTorus("torus0", math::float3(10.f, 0.0f, 0.0f), math::Quat::identity, 0.5f, 0.67f, 10, 30, App->model->color);
				App->model->materials.back().k_specular = 0.9f;
				App->model->materials.back().shininess = 64.0f;
				App->model->materials.back().k_specular = 0.6f;
				App->model->materials.back().k_diffuse = 0.5f;
				App->model->materials.back().k_ambient = 1.0f;
			}
			if (ImGui::MenuItem("Create Cube"))
			{
				App->model->CreateCube("cube0", math::float3(10.0f, 0.0f, 0.0f), math::Quat::identity, 2.0f, App->model->color);
				App->model->materials.back().k_specular = 0.9f;
				App->model->materials.back().shininess = 64.0f;
				App->model->materials.back().k_specular = 0.6f;
				App->model->materials.back().k_diffuse = 0.5f;
				App->model->materials.back().k_ambient = 1.0f;
			}
			if (ImGui::MenuItem("Create Cylinder"))
			{
				App->model->CreateCylinder("cylinder0", math::float3(10.0f, 0.0f, 0.0f), math::Quat::identity, 2.0f, 0.5f, 30, 30, App->model->color);
				App->model->materials.back().k_specular = 0.9f;
				App->model->materials.back().shininess = 64.0f;
				App->model->materials.back().k_specular = 0.6f;
				App->model->materials.back().k_diffuse = 0.5f;
				App->model->materials.back().k_ambient = 1.0f;
			}
			if (ImGui::MenuItem("Effects")) {
			}
			if (ImGui::MenuItem("Lights")) {
			}
			if (ImGui::MenuItem("Audio")) {
			}
			if (ImGui::MenuItem("Video")) {
			}
			if (ImGui::MenuItem("UI")) {
			}
			if (ImGui::MenuItem("Camera")) {
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Component")) {
			if (ImGui::MenuItem("Mesh")) {
			}
			if (ImGui::MenuItem("Effects")) {
			}
			if (ImGui::MenuItem("Physics")) {
			}
			if (ImGui::MenuItem("Audio")) {
			}
			if (ImGui::MenuItem("Video")) {
			}
			if (ImGui::MenuItem("UI")) {
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			ImGui::MenuItem(("Application"), (const char*)0, &showAppWindow);
			ImGui::MenuItem(("Scene"), (const char*)0, &showScene);
			ImGui::MenuItem(("Game"), (const char*)0, &showGame);
			ImGui::MenuItem(("Hierarchy"), (const char*)0, &showHierarchy);

			ImGui::MenuItem(("Inspector"), (const char*)0, &showInspector);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About")) {
			ImGui::MenuItem(("Help"), (const char*)0, &showHelpWindow);
			ImGui::MenuItem(("About Salsa"), (const char*)0, &showAboutWindow);
			if (ImGui::MenuItem(ICON_FA_JEDI" Repository")) {
				ShellExecuteA(NULL, "open", "https://github.com/JorxSS/SalsaEngine", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::EndMenu();
		}


	}
	ImGui::EndMainMenuBar();

}
void ModuleGUI::MainWindow() {
	ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::BeginTabBar("MainTabs");

		if (showScene)
			Scene();

		if (showGame)
			Game();

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void ModuleGUI::ShowTimeButtons() {
	if (ImGui::Begin("Time Buttons", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {

		ImVec2 time_window_size = ImGui::GetWindowSize();

		ImVec2 play_button_pos((time_window_size.x - 24)*0.5f - 26, (time_window_size.y - 24)*0.5f);
		ImGui::SetCursorPos(play_button_pos);

		if (ImGui::Button(ICON_FA_PLAY, ImVec2(24, 24)))
		{
			//TODO
		}ImGui::SameLine();

		if (ImGui::Button(ICON_FA_STOP, ImVec2(24, 24)))
		{
			//TODO
		}ImGui::SameLine();

		if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(24, 24)))
		{
			//TODO
		}
	}
	ImGui::End();
}


void ModuleGUI::Game() {
	if (ImGui::BeginTabItem(ICON_FA_GAMEPAD " Game"))
	{
		//isScene= ImGui::IsWindowFocused();
		sceneWidthGame = ImGui::GetWindowWidth();
		sceneHeightGame = ImGui::GetWindowHeight();
		App->renderer->DrawGame(sceneWidthGame, sceneHeightGame);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->sceneTex,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + sceneWidthGame, ImGui::GetCursorScreenPos().y + sceneHeightGame),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::EndTabItem();
	}
	
}

float ModuleGUI::GetSceneWidth() { return sceneWidth; }
float ModuleGUI::GetSceneHeight() { return sceneHeight; }

void ModuleGUI::Scene() {
	if (ImGui::BeginTabItem(ICON_FA_DICE_D20 " Scene"))
	{
		isScene = ImGui::IsWindowFocused();

		//ImVec2 vMin = ImGui::GetWindowWidth();
		//ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		sceneWidth = ImGui::GetWindowWidth();
		sceneHeight = ImGui::GetWindowHeight();

		//LOG("%0.1f %0.1f\n", sceneWidth, sceneHeight);

		scenePos.x = ImGui::GetWindowPos().x;
		scenePos.y = ImGui::GetWindowPos().y;

		/*vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;*/
		//ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));


		
		App->renderer->DrawScene(sceneWidth, sceneHeight);
		//LOG("Scene width: %0.1f, Scene Height: %0.1f", sceneWidth, sceneHeight);
		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->frameTex,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + sceneWidth, ImGui::GetCursorScreenPos().y + sceneHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		App->renderer->DrawGuizmo();
		ImGui::EndTabItem();
	}

}

ImVec2 ModuleGUI::GetScenePos() {
	return scenePos;
}

char* ModuleGUI::GetInputFile()//TODO Check if a texture is passed, not every item
{
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("png");

	if (GetOpenFileName(&ofn))
	{
		//MessageBox(NULL, szFileName, _T("It works!"), MB_OK);
		string temp = szFileName;

		//TODO: ADD HERE ACCEPETD FILETYPES
		if (temp.substr(temp.size() - 4) == ".png" || temp.substr(temp.size() - 4) == ".ttf")
			return szFileName;
		else
			return "Not valid";
	}
	else
		return "Not valid";
}



void ModuleGUI::GameObjecInfo() {
	/*if (ImGui::Begin(ICON_FA_INFO_CIRCLE" Old Inspector")) {
		isInspector = ImGui::IsWindowHovered();
		float width = ImGui::GetWindowWidth();
		float height = ImGui::GetWindowHeight();
		if (App->model->model) {
			if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform")) {

				ImGui::InputFloat3("Position", &App->model->modelPosition[0], 3, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat3("Rotation", &App->model->modelRotation[0], 3, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat3("Scale", &App->model->modelScale[0], 3, ImGuiInputTextFlags_ReadOnly);

			}
			if (ImGui::CollapsingHeader(ICON_FA_CUBES " Geometry")) {
				ImGui::Text("Meshes: %d", App->model->meshes.size());
				ImGui::InputInt("Triangles", &App->model->npolys, 0, 0, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputInt("Vertex", &App->model->nvertex, 0, 0, ImGuiInputTextFlags_ReadOnly);
				static bool wireframe = false;
				ImGui::Checkbox("Wireframe", &wireframe);
				App->renderer->SetWireframe(wireframe);

			}
			//Texture - changed to material
			/*if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Texture")) {

				static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

				static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
				int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
				for (unsigned int i = 0; i < App->model->textures_loaded.size(); i++) {
					// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
					ImGuiTreeNodeFlags node_flags = base_flags;
					std::string str = App->model->textures_loaded[i].path;
					const char *path = str.c_str();
					const bool is_selected = (selection_mask & (1 << i)) != 0;
					if (is_selected)
						node_flags |= ImGuiTreeNodeFlags_Selected;
					if (i < App->model->textures_loaded.size()) {
						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Texture %d", i);
						if (ImGui::IsItemClicked())
							node_clicked = i;
						if (node_open) {
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
				if (node_clicked != -1) {
					// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
						selection_mask = (1 << node_clicked);           // Click to single-select
				}

			}*/
}

void ModuleGUI::oldinspector() {
	if (ImGui::Begin(ICON_FA_GAMEPAD " Old inspector"))
	{
		if (ImGui::CollapsingHeader(ICON_FA_FILE_VIDEO " Shader")) {
			if (ImGui::Selectable("Flat Shading")) {

				App->model->shader = App->shader->flat_program;

			}
			if (ImGui::Selectable("Phong Shading")) {

				App->model->shader = App->shader->phong_program;

			}
			if (ImGui::Selectable("Blinn Shading")) {

				App->model->shader = App->shader->blinn_program;

			}
			if (ImGui::Selectable("Gouraud Shading")) {

				App->model->shader = App->shader->gouraud_program;

			}

		}

		
		if (ImGui::CollapsingHeader(ICON_FA_CLOCK " Timers")) {

			if (ImGui::CollapsingHeader("Time")) {
				ImGui::SliderFloat("Max FPS", &App->globalTimer->FPS, 10, 100);
				ImGui::Text("Limiting to %0.1f, mmeans each frame needs %0.1f ms to render", App->globalTimer->FPS, App->globalTimer->frameDelay);
				ImGui::Text("dt: %0.3f", App->globalTimer->dt);

			}

			if (ImGui::CollapsingHeader("Chrono")) {

				if (ImGui::Button("Play")) {
					App->mstimer->Start();
					LOG("STARTING TIMER! \n");
				}

				if (ImGui::Button("Read")) {
					LOG("Time elapsed since play was pressed: %0.2f s \n", App->mstimer->Read() / 1000);
				}

				if (ImGui::Button("Stop") && App->mstimer->isCounting) {
					LOG("Clock stopped. Final time: %0.2f s \n", App->mstimer->Stop() / 1000);
				}
			}


		}


		ImGui::End();
	}
}
void ModuleGUI::ShowHelp() {
	bool* p_open = NULL;
	if (ImGui::Begin("Help", p_open)) {

		ImGui::Text("USER GUIDE:");
		ImGui::ShowUserGuide();

	}
	ImGui::End();
}
void ModuleGUI::ShowAbout() {
	bool* p_open = NULL;
	if (ImGui::Begin("About", p_open)) {

		ImGui::Text("SALSA ENGINE 0.1");
		ImGui::Text("This Engine was created as a project for the Master Degree 'Advanced Programming for AAA Video Games' made in the UPC from Barcelona.");
		ImGui::Text("Authors:  "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 1, 1), "Jordi Sauras Salas");
		ImGui::Text("Libraries Used: ");
		static bool selection[7] = { false, false, false, false, false, false, false };
		if (ImGui::Selectable("SLD2 2.0.4", selection[0], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[0] = !selection[0];
				ShellExecuteA(NULL, "open", "https://www.libsdl.org/index.php", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("GLEW 2.1", selection[1], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[1] = !selection[1];
				ShellExecuteA(NULL, "open", "http://glew.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("ImGui", selection[2], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[2] = !selection[2];
				ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("MathGeoLib", selection[2], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[2] = !selection[2];
				ShellExecuteA(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("DevIL", selection[3], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[3] = !selection[3];
				ShellExecuteA(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("Assimp 3.3.1", selection[4], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[4] = !selection[4];
				ShellExecuteA(NULL, "open", "https://github.com/assimp/assimp", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("IconsFontCppHeaders", selection[5], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[5] = !selection[5];
				ShellExecuteA(NULL, "open", "https://github.com/juliettef/IconFontCppHeaders", NULL, NULL, SW_SHOWNORMAL);
			}
		if (ImGui::Selectable("FontAwesome", selection[6], ImGuiSelectableFlags_AllowDoubleClick))
			if (ImGui::IsMouseDoubleClicked(0)) {
				selection[6] = !selection[6];
				ShellExecuteA(NULL, "open", "https://fontawesome.com/", NULL, NULL, SW_SHOWNORMAL);
			}
		ImGui::Text("License: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "MIT");

	}
	ImGui::End();

}

	

void ModuleGUI::ShowDefWindow() {

	static bool fullscreen = false;
	static bool borderless = false;
	static bool resizable = true;
	static bool fsdesktop = false;
	static bool aspect = false;

	static float display_brightness = SDL_GetWindowBrightness(App->window->window);
	static int screen_w = 0;
	static int screen_h = 0;
	SDL_GetWindowSize(App->window->window, &screen_w, &screen_h);
	bool* p_open = NULL;
	if (ImGui::Begin(ICON_FA_COG " Configuration", p_open)) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		ImGui::Spacing();
		if (ImGui::CollapsingHeader(ICON_FA_TABLET_ALT " Application")) {
			ImGui::Text("Engine Version: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "0.1");
			ImGui::Separator();

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 100.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			int fps = ImGui::GetIO().Framerate;
			//Get frames
			if (frames.size() > 120) //Max seconds to show
			{
				for (size_t i = 1; i < frames.size(); i++) {
					frames[i - 1] = frames[i];
				}
				frames[frames.size() - 1] = fps;
			}
			else {
				frames.push_back(fps);
			}
			char title[25];
			sprintf_s(title, 25, "Framerate: %d", fps);
			ImGui::PlotHistogram("##framerate", &frames[0], frames.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		}
		if (ImGui::CollapsingHeader(ICON_FA_WINDOW_RESTORE " Window")) {

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
		if (ImGui::CollapsingHeader(ICON_FA_MICROCHIP" Hardware")) {
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
		if (ImGui::CollapsingHeader(ICON_FA_CAMERA_RETRO" Camera")) {
			ImGui::InputFloat3("Front", &App->scene->camera->frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Up", &App->scene->camera->frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Position", &App->scene->camera->frustum.pos[0], 3, ImGuiInputTextFlags_ReadOnly);

			ImGui::Separator();


			if (ImGui::SliderFloat("FOV", &App->scene->camera->frustum.horizontalFov, 0, 2 * 3.14f))
				App->scene->camera->SetFOV(App->scene->camera->frustum.horizontalFov);


			if (ImGui::SliderFloat("Aspect Ratio", &App->scene->camera->aspectRatio, 0, 10)) {
				if (!aspectFixed)
					App->scene->camera->SetAspectRatio(App->scene->camera->aspectRatio);
			}
			if (ImGui::SliderFloat("Camera Speed", &App->scene->camera->cameraSpeed, 0, 1))
				App->scene->camera->SetSpeed(App->scene->camera->cameraSpeed);

			if (ImGui::SliderFloat("Rotation Speed", &App->scene->camera->rotationSpeed, 0, 1))
				App->scene->camera->SetRotationSpeed(App->scene->camera->rotationSpeed);


			if (ImGui::SliderFloat("Near Plane", &App->scene->camera->frustum.nearPlaneDistance, 0, 1000)) {
				App->scene->camera->CalculateMatrixes();
			}
			if (ImGui::SliderFloat("Far Plane", &App->scene->camera->frustum.farPlaneDistance, 0, 1000)) {
				App->scene->camera->CalculateMatrixes();
			}
			static int clicked = 0;
			if (ImGui::Button("Reset Camera"))
				clicked++;
			if (clicked & 1) {
				App->scene->camera->SetRotationSpeed(ROTATION_SPEED);
				App->scene->camera->SetSpeed(CAMERA_SPEED);
				clicked = 0;
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Default Aspect Ratio", &aspect)) {
				if (showScene) {
					if (aspectFixed)
						aspectFixed = false;
					else {
						App->scene->camera->SetAspectRatio(sceneWidth / sceneHeight);
						aspectFixed = true;
					}


				}

			}
		}

		if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB" Light"))
		{
			/*ImGui::DragFloat3("Position", &App->model->light.pos[0], 3);
			if(ImGui::SliderFloat("Color R", &App->model->light.color.x, 0, 1)) {
				for (int i = 0; i < App->model->materials.size(); ++i)
					App->model->materials[i].diffuse_color = App->model->color;


			}
			if (ImGui::SliderFloat("Color G", &App->model->light.color.y, 0, 1)) {
				for (int i = 0; i < App->model->materials.size(); ++i)
					App->model->materials[i].diffuse_color = App->model->color;

			}
			if (ImGui::SliderFloat("Color B", &App->model->light.color.z, 0, 1)) {
				for (int i = 0; i < App->model->materials.size(); ++i)
					App->model->materials[i].diffuse_color = App->model->color;
			}

			if (ImGui::SliderFloat("Ambient", &App->model->light.ambient, 0, 1)) {

				glUniform1f(glGetUniformLocation(App->shader->def_program, "light.ambient"), App->model->light.ambient);

			}*/

			if (ImGui::DragFloat3("Position", &App->model->light.pos[0], 3)) {
				glUniform3f(glGetUniformLocation(App->shader->def_program, "light.position"), App->model->light.pos.x, App->model->light.pos.y, App->model->light.pos.z);

			}
			ImGui::SliderFloat("Color R", &App->model->light.color.x, 0, 1);
			ImGui::SliderFloat("Color G", &App->model->light.color.y, 0, 1);
			ImGui::SliderFloat("Color B", &App->model->light.color.z, 0, 1);

			glUniform3f(glGetUniformLocation(App->shader->def_program, "light.ambient"), App->model->light.color.x, App->model->light.color.y, App->model->light.color.z);


		}

		if (ImGui::CollapsingHeader(ICON_FA_CAMERA_RETRO" Game Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Front C", (float*)&App->renderer->GameCamera->frustum.front, 0.1);
			ImGui::DragFloat3("Up C", (float*)&App->renderer->GameCamera->frustum.up, 0.1);
			ImGui::DragFloat3("Position C", (float*)&App->renderer->GameCamera->frustum.pos, 0.1);

			ImGui::Separator();


		}
	}
	ImGui::End();
}
