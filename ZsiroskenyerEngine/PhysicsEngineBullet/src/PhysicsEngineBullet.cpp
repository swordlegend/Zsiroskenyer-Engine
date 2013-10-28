#include "PhysicsEngineBullet.h"

#include "..\..\Core\src\common.h"

#include "RigidTypeBullet.h"
#include "RigidEntityBullet.h"

#include <algorithm>
#include <list>
cPhysicsEngineBullet::cPhysicsEngineBullet() {
	//PHYSICSDEBUGDRAWER* drawer = new PHYSICSDEBUGDRAWER();

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	btVector3 worldMin(-1000, -1000, -1000);
	btVector3 worldMax( 1000,  1000,  1000);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btAxisSweep3(worldMin, worldMax);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	physicsWorld = new btSoftRigidDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	


	//GImpact �tk�z�s algot regizni kell a fizikai kontaktus v�grehajt�n�l..hogy tudjon r�la
	//M�g lehet hogy haszn�lok majd Gimpact �tkz�st...
	//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

	//drawer->setDebugMode(drawer->DBG_DrawAabb);
	//m_physicsworld->setDebugDrawer(drawer);
	
	physicsWorld->setGravity(btVector3(0 ,0 ,-200));

	physicsWorld->getDispatchInfo().m_useContinuous = true;
	//Ragad�s Spagetti effektus :D
	//m_physicsworld->getSolverInfo().m_splitImpulse=true;
	//m_physicsworld->getSolverInfo().m_numIterations = 20;
}

void cPhysicsEngineBullet::Release() {
		collisionShapes.clear();

	int i;
	for (i = physicsWorld->getNumCollisionObjects()-1; i>=0 ;i--) {
		btCollisionObject* obj = physicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();

		physicsWorld->removeCollisionObject(obj);
		SAFE_DELETE(obj);
	}

	SAFE_DELETE(physicsWorld);

	for(auto it = physicsTypes.begin() ; it != physicsTypes.end(); it++)
		SAFE_DELETE(*it);
	physicsTypes.clear();
	
	for(auto it = entities.begin() ; it != entities.end(); it++)
		SAFE_DELETE(*it);
	entities.clear();
}

void cPhysicsEngineBullet::SimulateWorld(float deltaT) {
	physicsWorld->stepSimulation(deltaT, 1);
}

IPhysicsEntity* cPhysicsEngineBullet::AddRigidEntity(const IPhysicsType* type, const Vec3& position) {
	
	cRigidTypeBullet* rigidType = (cRigidTypeBullet*)type;

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(position.x, position.y, position.z));

	// /Rigid things -> motionState
	// RigidType things mass, collisionShape, localInertia
	btRigidBody* body = new btRigidBody(rigidType->GetMass(), new btDefaultMotionState(trans), rigidType->GetCollisionShape(), rigidType->GetLocalInertia());
	physicsWorld->addRigidBody(body);
	cRigidEntityBullet * r = new cRigidEntityBullet(body);
	entities.push_back(r);
	return r;
}

IPhysicsType* cPhysicsEngineBullet::LoadRigidType(const zsString& geomPath, float mass, const cGeometryBuilder::tGeometryDesc* desc /*= NULL*/) {
	IPhysicsType* type;
	// Geom doesn't exists, create it
	if(! IsGeometryExists(geomPath)) {
		// Collision shape
		btCollisionShape* colShape = new btConvexHullShape((btScalar*)desc->vertices, desc->nVertices, desc->vertexStride);

		// New rigid Type
		type = new cRigidTypeBullet(colShape, mass);
		physicsTypes.push_back(type);
		return type;
	} else /*Geom exists*/ {
		// Search for equal massed type
		type = new cRigidTypeBullet(collisionShapes[geomPath], mass);
		auto it = find(physicsTypes.begin(), physicsTypes.end(), type);

		// Doesn't exists that mass
		if(it == physicsTypes.end())
			physicsTypes.push_back(type);
		else {
			delete type;
			type = *it;
		}
	}
	return type;
}

btRigidBody* cPhysicsEngineBullet::ShootBox(const Vec3& camPos,const Vec3& destination)
{
	if (physicsWorld) {
		btTransform asd;
		asd.setIdentity();
		asd.setOrigin(btVector3(camPos.x, camPos.y, camPos.z));

		float mass = 1.0f;
		btMotionState* motionstate = new btDefaultMotionState(asd);
		btCollisionShape* colshape = new btBoxShape(btVector3(10.0f,10.0f,10.0f));
		btVector3 localinertia(0.0f,0.0f,0.0f);
		btRigidBody* body = new btRigidBody(mass,motionstate,colshape,localinertia);
		
		//�rdekes �szrev�tel sajnos...
		//Csak azoknak a rigidbodykra lehet �j er�t stb k�sztetni, amelyikeknek utolj�ra
		//adtuk hozz� a RigidBody j�t a vil�ghoz...

		physicsWorld->addRigidBody(body);

		body->setLinearFactor(btVector3(1,1,1));
	
		btVector3 linVel(destination[0]-camPos[0],destination[1]-camPos[1],destination[2]-camPos[2]);

		body->setLinearVelocity(linVel);
		body->setAngularVelocity(btVector3(0,0,0));
		body->setContactProcessingThreshold(1e30);

		///when using m_ccdMode, disable regular CCD
		body->setCcdMotionThreshold(2.5f);
		body->setCcdSweptSphereRadius(0.4f);
		
		return body;
	}
	return NULL;
}

std::list<Vec3>* cPhysicsEngineBullet::GetCollisionShapeEdges() {
	
	// Final list
	std::list<Vec3>* edges = new std::list<Vec3>();

	// For Edge points...
	btVector3 p1;
	btVector3 p2;

	Vec3 fP1;
	Vec3 fP2;

	auto colObjArray = physicsWorld->getCollisionWorld()->getCollisionObjectArray();
	size_t nObjs = physicsWorld->getNumCollisionObjects();
	for(size_t i = 0; i < nObjs; i++) {
		btCollisionShape* colShape = colObjArray[i]->getCollisionShape();

		// Add each edge from convex Shape to the list
		if(colObjArray[i]->getCollisionShape()->isConvex()) {
			btConvexHullShape* convCol = (btConvexHullShape*)colShape;
			size_t nEdges = convCol->getNumEdges();
			for(size_t j = 0; j < nEdges; j++) {
				convCol->getEdge(j, p1, p2);
				edges->push_back(Vec3(p1.x(), p1.y(), p1.z()));
				edges->push_back(Vec3(p2.x(), p2.y(), p2.z()));
			}
			
		} else {
			//@TODO CONCAVE CollisionObject extraction to edge list...
		}
	}
	return edges;
}

bool cPhysicsEngineBullet::IsGeometryExists(const zsString& geomPath) {
	return collisionShapes.find(geomPath) != collisionShapes.end();
}