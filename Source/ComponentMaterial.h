#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"

class ComponentMaterial : public Component {
public:
	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial &material);
	~ComponentMaterial();

	void Update() override;
	void OnEditor() override;
};

#endif // __COMPONENTMATERIAL_H__
