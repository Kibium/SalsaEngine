#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

enum class Type;
class GameObject;

class Component {
public:
	Component();
	~Component();

	virtual void Enable();
	virtual update_status Update();
	virtual void Disable();
	virtual void OnEditor();

public:
	Type type;
	bool active = true;
	GameObject* myGo = nullptr;
	bool canDelete = true;
};

#endif // __COMPONENT_H__
