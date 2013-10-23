////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/math/math_all.h"

class cInstanceGroup;

class cGraphicsEntity {
	friend class cSceneManager;
public:
	cInstanceGroup* GetInstanceGroup();

	cGraphicsEntity();

	Vec3 position;
	Quat rotation;
	Vec3 scale;
	bool isVisible;
private:
	cInstanceGroup* instanceGroup;
};