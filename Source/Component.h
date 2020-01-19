#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "MathGeoLib.h"

enum class Type;
class GameObject;

typedef unsigned int uint32_t;

class Component {
public:
	Component();
	~Component();

	virtual void Enable();
	virtual update_status Update();
	virtual void Disable();
	virtual void OnEditor();

public:
	uint32_t UUID;
	Type type;
	bool active = true;
	GameObject* myGo = nullptr;
	bool canBeDeleted = true;

};

#endif // __COMPONENT_H__
