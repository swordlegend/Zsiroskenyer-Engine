#include "Core.h"

#include "common.h"
#include "Factory.h"

// TMP @TODO remove it
#include "Camera.h"
#include "Entity.h"

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

	// For debug purposes
	debugRenderer = new cRenderer(graphicsEngine->GetGraphicsApi(), graphicsEngine->GetShaderManager());
}

cCore::~cCore() {
	for(auto it = graphicsEntities.begin(); it != graphicsEntities.end(); it++)
		SAFE_DELETE(*it);

	for(auto it = physicsEntities.begin(); it != physicsEntities.end(); it++)
		SAFE_DELETE(*it);

	// Destroy engines
	SAFE_RELEASE(graphicsEngine);
	SAFE_RELEASE(physicsEngine);
	SAFE_DELETE(logicEngine);
}

void cCore::DebugRender(unsigned long renderFlags) {
	// Render Physics Triangles
	if(renderFlags & (unsigned long)eDebugRenderMode::PHYSICS_TRIANGLES) {
		Vec3* edges = new Vec3[30000];
		size_t nEdges;
		physicsEngine->GetCollisionShapeEdges(edges, 30000, nEdges);

		// Render lines for physics..
		cCamera* cam = graphicsEngine->GetActiveCamera();
		Matrix44 viewProj = cam->GetViewMatrix() * cam->GetProjMatrix();
		debugRenderer->RenderLines(viewProj, edges, nEdges, Vec3(1.0f, 0.0f, 0.0f));

		delete[] edges;
	}
}

void cCore::Update(float deltaT) {
	// Simulate physics world
	physicsEngine->SimulateWorld(deltaT);

	// Update Links
}

cEntity* cCore::AddEntity(const zsString& graphGeomPath,const zsString& physicsGeom, const zsString& mtlPath, float mass) {
	// Create entity module parts
	cGraphicsEntity* gEntity = graphicsEngine->CreateEntity(graphGeomPath, mtlPath);
	IPhysicsEntity* pEntity = physicsEngine->CreateRigidEntity(physicsGeom, mass);

	// Node for physics
	tTransNode nodeRootPhysics;
	nodeRootPhysics.trans = pEntity;

	// Node for graphics
	tTransNode nodeGraphics;
	nodeGraphics.trans = gEntity;

	// Link them
	nodeRootPhysics.childs.push_back(nodeGraphics);
	entityLinks.push_back(nodeRootPhysics);
	
	graphicsEntities.push_back(gEntity);
	physicsEntities.push_back(pEntity);
	cEntity* e = new cEntity(gEntity, pEntity);
	return e;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
