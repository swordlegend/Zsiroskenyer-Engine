////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that manages the 3D scene's entities.
//	This includes, but is not limited to creation/deletion of entities,
//	positioning entities, talking to the resource manager, grouping entities
//	by instancing and/or spatial position.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "../../Core/src/zsString.h"

#include "InstanceGroup.h"
#include "ShadowMap.h"
#include "GraphicsLight.h"

#include <unordered_set>
#include <utility>


class cGraphicsEntity;
class cResourceManager;
class cCamera;

// helpers for storing cInstanceGroups by pointer
struct cInstGroupPtrHasher {
	size_t operator()(cInstanceGroup* i) {
		return i!=nullptr?std::hash<cInstanceGroup>()(*i):0;
	}
};
struct cInstGroupPtrCompare {
	bool operator()(cInstanceGroup* a, cInstanceGroup* b) {
		if (a==nullptr || b==nullptr)
			return false;
		return *a==*b;
	}
};

// scene manager
class cSceneManager {
public:
	cSceneManager();
	~cSceneManager();

	cGraphicsEntity* AddEntity(cGeometryRef geom, cMaterialRef mtl);
	void RemoveEntity(const cGraphicsEntity* entity);
	std::pair<cGraphicsLight, cShadowMap>* AddLight();
	void RemoveLight(const std::pair<cGraphicsLight, cShadowMap>* light);
	void Clear();

	const std::unordered_set<cInstanceGroup*, cInstGroupPtrHasher,cInstGroupPtrCompare>& GetInstanceGroups() const;
	const std::unordered_set<std::pair<cGraphicsLight, cShadowMap>*>& GetLights() const;
private:
	std::unordered_set<cInstanceGroup*,	cInstGroupPtrHasher,cInstGroupPtrCompare> instanceGroups;
	std::unordered_set<std::pair<cGraphicsLight, cShadowMap>*> lights;
};