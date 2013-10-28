#include "Core.h"

#include "common.h"
#include "Factory.h"

#include "GeometryBuilder.h"

cCore* cCore::instance = NULL;

cCore* cCore::GetInstance() {
	if(cCore::instance == NULL)
		cCore::instance = new cCore();
	return cCore::instance;
}

cCore::cCore() {
	graphicsEngine = Factory.CreateGraphicsEngine();
	physicsEngine = Factory.CreatePhysicsEngineBullet();
	logicEngine = new cLogicEngine();
}

cCore::~cCore() {
	SAFE_RELEASE(graphicsEngine);
	SAFE_RELEASE(physicsEngine);
	SAFE_DELETE(logicEngine);
}

void cCore::Update(float deltaT) {
	physicsEngine->SimulateWorld(deltaT);
}

cEntityType* cCore::CreateEntityType(const zsString& name, const zsString& physGraphGeomPath, const zsString& mtlPath, float mass /*= 0.0f*/) {
	// EntityType doesn't exists
	if(! logicEngine->IsEntityTypeExits(name)) {
		cResourceManager* graphicsRMgr = graphicsEngine->GetResourceManager();

		// Material
		cMaterialRef* mtl = graphicsRMgr->LoadMaterial(mtlPath);
		cGeometryRef* geom = NULL;
		IPhysicsType* physicsType = NULL;

		// Graphics geometry doesn't exists
		if(! graphicsRMgr->IsGeometryExists(physGraphGeomPath)) {
			cGeometryBuilder builder;
			cGeometryBuilder::tGeometryDesc desc = builder.LoadGeometryDAE(physGraphGeomPath);
			geom = graphicsRMgr->LoadGeometry(physGraphGeomPath, desc);
			physicsType = physicsEngine->LoadRigidType(physGraphGeomPath, desc, mass);
		} else {
			geom = graphicsRMgr->LoadGeometry(physGraphGeomPath);
			physicsType = physicsEngine->GetRigidType(physGraphGeomPath);
		}

		return logicEngine->CreateEntityType(name, geom, mtl, physicsType);
	}
	return NULL;
}

cEntity* cCore::AddEntity(cEntityType* type, const Vec3& position) {
	cGraphicsEntity* gEntity = graphicsEngine->GetSceneManager()->AddEntity(type->GetGraphicsGeometry(), type->GetMaterial());
	IPhysicsEntity* pEntity = NULL;//physicsEngine->AddRigidEntity(type->NULL;//physicsEngine->AddEntity(

	cEntity* e = logicEngine->AddEntity(gEntity, pEntity);
		e->SetPosition(position);
	return e;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
