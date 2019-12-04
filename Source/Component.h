#ifndef __COMPONENT_H__
#define __COMPONENT_H__

enum Type;
class GameObject;

class Component {
public:
	Component();
	~Component();

	virtual void Enable();
	virtual void Update();
	virtual void Disable();

public:
	Type type;
	bool active;
	GameObject* my_go = nullptr;
};

#endif // __COMPONENT_H__
