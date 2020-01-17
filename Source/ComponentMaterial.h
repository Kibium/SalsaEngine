#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentMaterial : public Component {
public:
	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial &material);
	~ComponentMaterial();

	update_status Update() override;
	void UpdateMaterial(float4 &color);
	void UpdateMaterial(float k);
	void UpdateMaterial(unsigned int& materialID);
	void OnEditor() override;
};

#endif // __COMPONENTMATERIAL_H__
