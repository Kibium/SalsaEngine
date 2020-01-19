#include "ComponentMaterial.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleShader.h"

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

void ComponentMaterial::UpdateMaterial(unsigned int& materialID) {

	const char* dir = App->gui->GetInputFile();
	if (dir != "Not valid")
		materialID = App->texture->Load(dir);

}

void ComponentMaterial::UpdateMaterial(float4 &color) {

	color = myGo->model->mat.diffuse_color;

}

void ComponentMaterial::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Material", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentMaterial", &active)) {
			active ? Enable() : Disable();
		}
		if (myGo != nullptr && myGo->model != nullptr) {

			ImGui::Text("   Diffuse");	ImGui::SameLine(); ImGui::Text("    Occlusion"); ImGui::SameLine(); ImGui::Text("    Specular");
			if (ImGui::ImageButton((void*)(intptr_t)myGo->model->mat.diffuse_map, ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0))) {

				UpdateMaterial(myGo->model->mat.diffuse_map);

			} ImGui::SameLine();

			if (ImGui::ImageButton((void*)(intptr_t)myGo->model->mat.occlusion_map, ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0))) {

				UpdateMaterial(myGo->model->mat.occlusion_map);

			} ImGui::SameLine();

			if (ImGui::ImageButton((void*)(intptr_t)myGo->model->mat.specular_map, ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0))) {

				UpdateMaterial(myGo->model->mat.specular_map);
			}

			if (ImGui::TreeNodeEx("Diffuse")) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Path:");
				ImGui::SameLine();
				ImGui::Text(myGo->model->mat.diff_path.c_str());
				ImGui::Spacing();

				ImGui::Image((void*)(intptr_t)myGo->model->mat.diffuse_map, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::PushItemWidth(100);

				ImGui::SliderFloat("Color R", &myGo->model->mat.diffuse_color.x, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color G", &myGo->model->mat.diffuse_color.y, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color B", &myGo->model->mat.diffuse_color.z, 0, 1);

				UpdateMaterial(myGo->model->mat.diffuse_color);
				glUniform4f(glGetUniformLocation(App->shader->def_program, "material.diff_color"),
					myGo->model->mat.diffuse_color.x,
					myGo->model->mat.diffuse_color.y,
					myGo->model->mat.diffuse_color.z,
					myGo->model->mat.diffuse_color.w);

				if (ImGui::SliderFloat("k diffuse", &myGo->model->mat.k_diffuse, 0, 1)) {
					glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_diff"), myGo->model->mat.k_diffuse);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Occlusion")) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Path:");
				ImGui::SameLine();
				ImGui::Text(myGo->model->mat.occ_path.c_str());
				ImGui::Spacing();

				ImGui::Image((void*)(intptr_t)myGo->model->mat.occlusion_map, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::PushItemWidth(100);
				ImGui::SliderFloat("Color R", &myGo->model->mat.occlusion_color.x, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color G", &myGo->model->mat.occlusion_color.y, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color B", &myGo->model->mat.occlusion_color.z, 0, 1);


				glUniform4f(glGetUniformLocation(App->shader->def_program, "material.occ_color"), myGo->model->mat.occlusion_color.x, myGo->model->mat.occlusion_color.y, myGo->model->mat.occlusion_color.z, myGo->model->mat.occlusion_color.w);

				if (ImGui::SliderFloat("k ambient", &myGo->model->mat.k_ambient, 0, 1)) {
					glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_occ"), myGo->model->mat.k_ambient);

				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Specular")) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Path:");
				ImGui::SameLine();
				ImGui::Text(myGo->model->mat.spec_path.c_str());
				ImGui::Spacing();

				ImGui::Image((void*)(intptr_t)myGo->model->mat.specular_map, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::PushItemWidth(100);
				ImGui::SliderFloat("Color R", &myGo->model->mat.specular_color.x, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color G", &myGo->model->mat.specular_color.y, 0, 1); ImGui::SameLine();
				ImGui::SliderFloat("Color B", &myGo->model->mat.specular_color.z, 0, 1);


				glUniform4f(glGetUniformLocation(App->shader->def_program, "material.spec_color"), myGo->model->mat.specular_color.x, myGo->model->mat.specular_color.y, myGo->model->mat.specular_color.z, myGo->model->mat.specular_color.w);

				if (ImGui::SliderFloat("k specular", &myGo->model->mat.k_specular, 0, 1)) {
					glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_spec"), myGo->model->mat.k_specular);

				}

				ImGui::TreePop();
			}

			if (ImGui::SliderFloat("Shininess", &myGo->model->mat.shininess, -100, 100)) {
				glUniform1f(glGetUniformLocation(App->shader->def_program, "material.shininess"), myGo->model->mat.shininess);

			}

		}
	}
	ImGui::Separator();
}
