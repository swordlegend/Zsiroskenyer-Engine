// Entity.h By Zsíroskenyér Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "IPhysicsEntity.h"

class cEntity {
public:
	cEntity(cGraphicsEntity* g, IPhysicsEntity* p);

	void SetPos(const Vec3& pos);
	void SetRotation(const Quat& rot);

	cGraphicsEntity* GetGraphicsEntity();
	IPhysicsEntity* GetPhysicsEntity();

protected:
	cGraphicsEntity* graphicsEntity;
	IPhysicsEntity* physicsEntity;
};