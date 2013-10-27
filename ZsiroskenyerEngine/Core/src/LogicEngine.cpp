#include "LogicEngine.h"

bool cLogicEngine::IsEntityTypeExits(const zsString& str) {
	return entityTypes.find(str) != entityTypes.end();
}

cEntityType* cLogicEngine::CreateEntityType(const zsString& name, cGeometryRef *graphicsGeom, cMaterialRef *material, float mass) {
	cEntityType* newType = new cEntityType(graphicsGeom, material, mass);
	entityTypes[name] = newType;
	return newType;
}

cEntity* cLogicEngine::AddEntity(cGraphicsEntity* gEntity, IPhysicsEntity* pEntity) {
	cEntity* e = new cEntity(gEntity, pEntity);
	entities.push_back(e);
	return e;
}