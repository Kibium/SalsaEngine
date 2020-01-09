#include "ComponentMaterial.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"

ComponentMaterial::ComponentMaterial() {
	type = Type::MATERIAL;
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial &material) {
}

ComponentMaterial::~ComponentMaterial() {
}

update_status ComponentMaterial::Update() {
	return UPDATE_CONTINUE;
}

void ComponentMaterial::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Material", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentMaterial", &active)) {
			active ? Enable() : Disable();
		}
		if (myGo != nullptr) {
			static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

			static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
			int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
			for (unsigned int i = 0; i < myGo->model->textures_loaded.size(); i++) {
				// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
				ImGuiTreeNodeFlags node_flags = base_flags;
				std::string str = myGo->model->textures_loaded[i].path;
				const char *path = str.c_str();
				const bool is_selected = (selection_mask & (1 << i)) != 0;
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;
				if (i < myGo->model->textures_loaded.size()) {
					bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Texture %d", i);
					if (ImGui::IsItemClicked())
						node_clicked = i;
					if (node_open) {
						ImGui::Text("Path:");
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), path);

						ImGui::Text("Texture %d size:", i);

						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d x %d", myGo->model->textureWidth[i], myGo->model->textureHeight[i]);


						ImGui::Spacing();
						float width = ImGui::GetWindowWidth();
						ImGui::Image((void*)(intptr_t)myGo->model->textures_loaded[i].id, ImVec2(width*0.5, width*0.5), ImVec2(0, 1), ImVec2(1, 0));
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
	ImGui::Separator();
}
