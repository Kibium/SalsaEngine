#ifndef __COMPONENT_H__
#define __COMPONENT_H__

enum class Type;
class GameObject;

class Component {
public:
	Component();
	~Component();

	virtual void Enable();
	virtual void Update() {};
	virtual void Disable();
	virtual void OnEditor() = 0;

public:
	int ID = 0;
	Type type;
	bool active;
	GameObject* myGo = nullptr;
};

#endif // __COMPONENT_H__
