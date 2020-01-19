#include "JsonConfig.h"
#include "Globals.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include "Component.h"
#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

JsonConfig::JsonConfig() {
	document.SetObject();
	allocator = &document.GetAllocator();
	gameObjects = rapidjson::Value(rapidjson::kArrayType);
}

JsonConfig::~JsonConfig() {
}

void JsonConfig::SaveJson(const char *fileName) {

	document.AddMember("Game Objects", gameObjects, *allocator);

	// create and log json result
	rapidjson::StringBuffer strbuf;
	strbuf.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);
	LOG(strbuf.GetString());

	// save json to file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "wt");

	if (!file)
		LOG("Error saving scene. Can not create %s file.", fileName);

	char *jsonFile = _strdup(strbuf.GetString());

	fwrite(jsonFile, sizeof(char), strlen(jsonFile), file);
	fclose(file);

	LOG("Saved scene data!\n");
}

void JsonConfig::SaveGameObject(const GameObject& obj) {

	// object properties
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("UID", obj.UUID, *allocator);
	object.AddMember("Index", obj.modelIndex, *allocator);
	object.AddMember("Name", rapidjson::Value(obj.name.c_str(), *allocator), *allocator);
	object.AddMember("Active", obj.isActive, *allocator);
	object.AddMember("Static", obj.isStatic, *allocator);
	if (obj.model != nullptr) {
		//LOG("MODEL FILE PATH: %s\n", obj.modelPath.c_str());
		object.AddMember("Model", rapidjson::Value(obj.modelPath.c_str(), *allocator), *allocator);
	}
	else {
		object.AddMember("Model", "None", *allocator);
	}
	if (obj.transform != nullptr) {
		object.AddMember("TransX", obj.transform->position.x, *allocator);
		object.AddMember("TransY", obj.transform->position.y, *allocator);
		object.AddMember("TransZ", obj.transform->position.z, *allocator);
		object.AddMember("RotX", obj.transform->rotationFloat.x, *allocator);
		object.AddMember("RotY", obj.transform->rotationFloat.y, *allocator);
		object.AddMember("RotZ", obj.transform->rotationFloat.z, *allocator);
		object.AddMember("ScaleX", obj.transform->scale.x, *allocator);
		object.AddMember("ScaleY", obj.transform->scale.y, *allocator);
		object.AddMember("ScaleZ", obj.transform->scale.z, *allocator);
	}

	// object's components properties
	rapidjson::Value components(rapidjson::kArrayType);
	rapidjson::Value camera(rapidjson::kObjectType);
	rapidjson::Value transform(rapidjson::kObjectType);
	rapidjson::Value mesh(rapidjson::kObjectType);
	rapidjson::Value material(rapidjson::kObjectType);

	if (obj.transform != nullptr) {
		transform.AddMember("UID", obj.transform->UUID, *allocator);
		transform.AddMember("ParentUID", obj.transform->myGo->UUID, *allocator);
		transform.AddMember("Type", (int)obj.transform->type, *allocator);
		transform.AddMember("Active", obj.transform->active, *allocator);
		rapidjson::Value trans(rapidjson::kArrayType);
		trans.PushBack(obj.transform->position.x, *allocator).PushBack(obj.transform->position.y, *allocator).PushBack(obj.transform->position.z, *allocator);
		transform.AddMember("Translation", trans, *allocator);
		rapidjson::Value rot(rapidjson::kArrayType);
		rot.PushBack(obj.transform->rotationFloat.x, *allocator).PushBack(obj.transform->rotationFloat.y, *allocator).PushBack(obj.transform->rotationFloat.z, *allocator);
		transform.AddMember("Rotation", rot, *allocator);
		rapidjson::Value scale(rapidjson::kArrayType);
		scale.PushBack(obj.transform->scale.x, *allocator).PushBack(obj.transform->scale.y, *allocator).PushBack(obj.transform->scale.z, *allocator);
		transform.AddMember("Scale", scale, *allocator);
		components.PushBack(transform, *allocator);
	}

	if (obj.mesh != nullptr) {
		mesh.AddMember("UID", obj.mesh->UUID, *allocator);
		mesh.AddMember("ParentUID", obj.mesh->myGo->UUID, *allocator);
		mesh.AddMember("Type", (int)obj.mesh->type, *allocator);
		mesh.AddMember("Active", obj.mesh->active, *allocator);
		components.PushBack(mesh, *allocator);
	}

	if (obj.material != nullptr) {
		material.AddMember("UID", obj.material->UUID, *allocator);
		material.AddMember("ParentUID", obj.material->myGo->UUID, *allocator);
		material.AddMember("Type", (int)obj.material->type, *allocator);
		material.AddMember("Active", obj.material->active, *allocator);
		components.PushBack(material, *allocator);
	}

	object.AddMember("Components", components, *allocator);
	gameObjects.PushBack(object, *allocator);
}

void JsonConfig::LoadJson(const char *fileName) {

	// load json from file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "rt");

	if (!file)
		LOG("Error loasing scene. Can not read %s file.", fileName);

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char *json = new char[size + 1];
	fread(json, 1, size, file);
	json[size] = 0;
	fclose(file);

	// parse
	document.Parse(json);
	assert(document.IsObject());

	const rapidjson::Value& a = document["Game Objects"];
	assert(a.IsArray());
	rapidjson::SizeType i;
	static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

	// loop game objects
	for (i = 0; i < a.Size(); ++i) {
		LOG("Game Object %d:\n", i);
		GameObject *gameObject = App->scene->CreateGameObject();
		math::float3 tempTrans;
		math::float3 tempRot;
		math::float3 tempScale;

		for (rapidjson::Value::ConstMemberIterator itr = a[i].MemberBegin(); itr != a[i].MemberEnd(); ++itr) {
			LOG("%s : ", itr->name.GetString());

			if (std::string(itr->name.GetString()) == std::string("UID")) {
				gameObject->UUID = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("Index")) {
				gameObject->modelIndex = itr->value.GetFloat();
			}
			if (std::string(itr->name.GetString()) == std::string("Name")) {
				gameObject->name = itr->value.GetString();
			}
			else if (std::string(itr->name.GetString()) == std::string("Active")) {
				gameObject->isActive = itr->value.GetBool();
			}
			else if (std::string(itr->name.GetString()) == std::string("Static")) {
				gameObject->isStatic = itr->value.GetBool();
			}
			else if (std::string(itr->name.GetString()) == std::string("Model")) {
				gameObject->modelPath = itr->value.GetString();
				if (gameObject->modelPath != "None") {
					Model* modelContainer = new Model(gameObject->modelPath.c_str(), false);
					gameObject->modelContainer = modelContainer;
					gameObject->model = modelContainer->meshes[gameObject->modelIndex];
				}
			}
			else if (std::string(itr->name.GetString()) == std::string("TransX")) {
				tempTrans.x = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("TransY")) {
				tempTrans.y = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("TransZ")) {
				tempTrans.z = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("RotX")) {
				tempRot.x = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("RotY")) {
				tempRot.y = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("RotZ")) {
				tempRot.z = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("ScaleX")) {
				tempScale.x = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("ScaleY")) {
				tempScale.y = itr->value.GetFloat();
			}
			else if (std::string(itr->name.GetString()) == std::string("ScaleZ")) {
				tempScale.z = itr->value.GetFloat();
				gameObject->transform->myGo = gameObject;
				gameObject->transform->SetTransform(tempTrans, tempRot, tempScale);
				gameObject->CreateComponent(Type::MESH);
				gameObject->CreateComponent(Type::MATERIAL);
			}

			std::string memberType = kTypeNames[itr->value.GetType()];
			const char *member = memberType.c_str();

			if (memberType == "Null") {
				LOG("Null");

			}
			else if (memberType == "False" || memberType == "True") {
				LOG("%d", itr->value.GetBool());

			}
			else if (memberType == "Object") {
				LOG("Object");

			}
			else if (memberType == "Array") {
				LOG("Array");
				//for (rapidjson::Value::ConstValueIterator a = itr.Begin(); a != itr.End(); ++itr) {
				//	printf("%d ", itr->GetInt());

				//}

			}
			else if (memberType == "String") {
				LOG("%s", itr->value.GetString());

			}
			else if (memberType == "Number") {
				LOG("%f", itr->value.GetFloat());

			}

			LOG("\n");
		}

		LOG("\n");
	}

	LOG("Game Objects Count: %d\n", i);

	delete json;
	LOG("Loaded scene data!\n");
}
