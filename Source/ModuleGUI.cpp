#include "ModuleGUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
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
#include "GameObject.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentTransform.h"

struct TestObject {
	bool selected;
	std::string name;
	std::vector<TestObject*> test_object_child;
};

std::vector<TestObject*> test_object;

void ClearAllSelection() {
	for (auto &elem : test_object) {
		elem->selected = false;
	}
}

void DrawChild(TestObject * obj) {
	//ImGui::TreeNodeEx(obj->name.c_str());
	//LOG("%d\n", obj->name.c_str());
	//ImGui::TreeNodeEx("test");
	//ImGui::TreePop();
}

void Draw() {
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	for (int i = 0; i < test_object.size(); ++i) {

		bool node_open = ImGui::TreeNodeEx(test_object[i]->name.c_str(), node_flags, test_object[i]->name.c_str());

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MoveToAdd")) {
				TestObject * test = new TestObject();
				test->selected = false;
				test->name = *(static_cast<std::string*>(payload->Data));
				LOG("Name: %s\n", static_cast<std::string*>(payload->Data));
				test_object[i]->test_object_child.push_back(test);
				test_object.erase(test_object.begin() + i + 1);
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::BeginDragDropSource(node_flags)) {
			std::string sent_data = test_object[i]->name;
			ImGui::SetDragDropPayload("MoveToAdd", &sent_data, sent_data.size());
			ImGui::Text("Move to...");
			ImGui::EndDragDropSource();
		}
		if (ImGui::IsItemHovered()) {
			if (ImGui::GetIO().KeyShift && ImGui::IsMouseReleased(0)) {
				// some shift click
			}
			else if (ImGui::GetIO().KeyCtrl && ImGui::IsMouseReleased(0)) {
				// some control click function
			}
			else if (ImGui::IsMouseReleased(0)) {
				ClearAllSelection();
				test_object[i]->selected = true;
			}
		}
		if (node_open) {
			for (auto &child_elem : test_object[i]->test_object_child) {
				DrawChild(child_elem);
			}
			ImGui::TreePop();
		}
	}
}

// https://github.com/ocornut/imgui/issues/2597
// https://github.com/ocornut/imgui/issues/2209
// https://discourse.dearimgui.org/t/tree-nodes-help/82/3
// https://github.com/CITMProject3/Project3/blob/master/Hierarchy.cpp

ModuleGUI::ModuleGUI() {
}


ModuleGUI::~ModuleGUI() {
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

	// Hierarchy
	//GameObject* obj1 = new GameObject();
	//obj1->name = "Obj1";

	//GameObject* obj2 = new GameObject();
	//obj2->name = "Obj2";

	//GameObject* obj3 = new GameObject();
	//obj3->name = "Obj3";

	//App->scene->game_objects.push_back(obj1);
	//App->scene->game_objects.push_back(obj2);
	//App->scene->game_objects.push_back(obj3);


	return true;
}

update_status ModuleGUI::PreUpdate() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::Update() {

	MainMenu();
	if (showAppWindow)
		ShowDefWindow();
	if (showHelpWindow)
		ShowHelp();
	if (showScene)
		Scene();
	if (showInspector)
		GameObjecInfo();
	if (showAboutWindow)
		ShowAbout();

	ShowConsole(ICON_FA_TERMINAL " Console");

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

void ModuleGUI::Scene() {
	if (ImGui::Begin(ICON_FA_DICE_D20 " Scene")) {
		isScene = ImGui::IsWindowFocused();
		sceneWidth = ImGui::GetWindowWidth();
		sceneHeight = ImGui::GetWindowHeight();
		App->renderer->DrawScene(sceneWidth, sceneHeight);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->frameTex,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + sceneWidth, ImGui::GetCursorScreenPos().y + sceneHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

	}
	ImGui::End();
}

void ModuleGUI::GameObjecInfo() {
	if (ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector")) {
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
				ImGui::InputInt("Mesh", &App->model->nmeshes, 0, 0, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputInt("Triangles", &App->model->npolys, 0, 0, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputInt("Vertex", &App->model->nvertex, 0, 0, ImGuiInputTextFlags_ReadOnly);
				static bool wireframe = false;
				ImGui::Checkbox("Wireframe", &wireframe);
				App->renderer->SetWireframe(wireframe);

			}
			if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Texture")) {

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

			}
		}

	}
	ImGui::End();
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
			ImGui::InputFloat3("Front", &App->camera->frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Up", &App->camera->frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Position", &App->camera->frustum.pos[0], 3, ImGuiInputTextFlags_ReadOnly);

			ImGui::Separator();


			if (ImGui::SliderFloat("FOV", &App->camera->frustum.horizontalFov, 0, 2 * 3.14f))
				App->camera->SetFOV(App->camera->frustum.horizontalFov);

			if (ImGui::SliderFloat("Aspect Ratio", &App->camera->aspectRatio, 0, 10)) {
				if (!aspectFixed)
					App->camera->SetAspectRatio(App->camera->aspectRatio);
			}
			if (ImGui::SliderFloat("Camera Speed", &App->camera->cameraSpeed, 0, 1))
				App->camera->SetSpeed(App->camera->cameraSpeed);

			if (ImGui::SliderFloat("Rotation Speed", &App->camera->rotationSpeed, 0, 1))
				App->camera->SetRotationSpeed(App->camera->rotationSpeed);

			static int clicked = 0;
			if (ImGui::Button("Reset Camera"))
				clicked++;
			if (clicked & 1) {
				App->camera->SetRotationSpeed(ROTATION_SPEED);
				App->camera->SetSpeed(CAMERA_SPEED);
				clicked = 0;
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Default Aspect Ratio", &aspect)) {
				if (showScene) {
					if (aspectFixed)
						aspectFixed = false;
					else {
						App->camera->SetAspectRatio(sceneWidth / sceneHeight);
						aspectFixed = true;
					}


				}

			}
		}

		App->scene->game_objects[0]->Draw();
		if (ImGui::IsItemHovered()) {
			App->scene->game_objects[0]->selected = !App->scene->game_objects[0]->selected;
		}
	}
	ImGui::End();
	ImGui::ShowDemoWindow();


	////////////////////////////////////////////////////////////
	if (ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector2")) {

		for (int i = 0; i < App->scene->game_objects.size(); ++i) {
			if (App->scene->game_objects[i]->selected)
				App->scene->game_objects[i]->Update();
		}

		ImGui::End();
	}
}
