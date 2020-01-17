#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransform : public Component {
public:
	ComponentTransform();
	~ComponentTransform();

	update_status Update() override;
	void OnEditor() override;

public:

};

#endif // __COMPONENTTRANSFORM_H__