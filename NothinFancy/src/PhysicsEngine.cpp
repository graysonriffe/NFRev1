#include "PhysicsEngine.h"

#include "Application.h"
#include "Entity.h"
#include "Utility.h"

//Remove this
#include "Input.h"

namespace nf {
	class PhysicsErrorCallback : public PxErrorCallback {
		virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
#ifdef _DEBUG
			Debug::ErrorImp(message, file, line);
			__debugbreak();
#else
			Error(message);
#endif
		}
	};

	PhysicsEngine::PhysicsEngine(Application* app) :
		m_app(app),
		m_foundation(nullptr),
		m_pvd(nullptr),
		m_phy(nullptr),
		m_cooking(nullptr),
		m_dispacher(nullptr),
		m_scene(nullptr),
		m_err(nullptr),
		m_stepSize(1.0 / 60.0),
		m_accumulator(0.0)
	{
		m_err = new PhysicsErrorCallback;
		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_alloc, *m_err);
		if (!m_foundation)
			Error("Could not initialize physics engine!");

#ifdef _DEBUG
		m_pvd = PxCreatePvd(*m_foundation);
		m_transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		m_pvd->connect(*m_transport, PxPvdInstrumentationFlag::eALL);
#endif

		m_phy = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), false, m_pvd);
		if (!m_phy)
			Error("Could not initialize physics engine!");

		m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(PxTolerancesScale()));
		if (!m_cooking)
			Error("Could not initialize physics engine!");

		m_dispacher = PxDefaultCpuDispatcherCreate(2);
	}

	void PhysicsEngine::newScene() {
		PxSceneDesc desc(m_phy->getTolerancesScale());
		desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		desc.cpuDispatcher = m_dispacher;
		desc.filterShader = PxDefaultSimulationFilterShader;
		m_scene = m_phy->createScene(desc);
		m_scene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

		PxPvdSceneClient* cli = m_scene->getScenePvdClient();
		if (cli) {
			cli->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			cli->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			cli->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		//TODO: Delete this
		PxMaterial* mat = m_phy->createMaterial(0.5f, 0.5f, 0.6f);
		PxRigidStatic* ground = PxCreatePlane(*m_phy, PxPlane(0, 1, 0, 0), *mat);
		m_scene->addActor(*ground);
	}

	void PhysicsEngine::update(double dt) {
		//TODO: Remove this
		static bool start = false;
		if (m_app->isKeyPressed(NFI_ENTER))
			start = true;
		if (!start) return;

		if (!m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC)) return;

		unsigned int count = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
		PxActor** actors = new PxActor * [count];
		m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, count);
		//TODO: Add static actors here

		if (m_app->isKeyPressed(NFI_F)) {
			Vec3 pos = m_app->getCurrentState()->getCamera()->getPosition();
			PxVec3 camPos((float)pos.x, (float)pos.y, (float)pos.z);
			PxQuat q(PxIdentity);
			((PxRigidDynamic*)actors[0])->setGlobalPose(PxTransform(camPos, q));

			Vec3 camDir = m_app->getCurrentState()->getCamera()->getRotation();
			float speed = 100.0f;
			PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)actors[0], 1000.0);
			((PxRigidDynamic*)actors[0])->setLinearVelocity(PxVec3((float)camDir.x * speed, (float)camDir.y* speed, (float)camDir.z * speed));
		}

		for (unsigned int i = 0; i < count; i++) {
			Entity* currEnt = (Entity*)actors[i]->userData;
			if (!currEnt->needsPhysicsUpdate()) continue;

			Vec3 pos = currEnt->getPosition();
			PxVec3 pos2((float)pos.x, (float)pos.y, (float)pos.z);
			Vec4 quat = currEnt->getRotation();
			PxQuat quat2(quat.x, quat.y, quat.z, quat.w);
			((PxRigidDynamic*)actors[i])->setGlobalPose(PxTransform(pos2, quat2));
		}
		delete[] actors;

		m_accumulator += dt;
		unsigned int stepCount = (unsigned int)(m_accumulator / m_stepSize);
		m_accumulator -= stepCount * m_stepSize;
		for (unsigned int i = 0; i < stepCount; i++) {
			m_scene->simulate((float)m_stepSize);
			m_scene->fetchResults(true);
		}

		PxActor** actives = m_scene->getActiveActors(count);
		//TODO: Kinematic actors?
		for (unsigned int i = 0; i < count; i++) {
			Entity& ent = *(Entity*)actives[i]->userData;
			PxTransform transform = ((PxRigidActor*)actives[i])->getGlobalPose();
			ent.setPositionPhysics(Vec3(transform.p.x, transform.p.y, transform.p.z));
			ent.setRotationPhysics(Vec4(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
		}
	}

	void PhysicsEngine::addActor(Entity* entity) {
		Entity::Type type = entity->getType();

		Vec3 pos = entity->getPosition();
		PxMaterial* mat = m_phy->createMaterial(0.5f, 0.5f, 0.0f);
		PxRigidDynamic* act = PxCreateDynamic(*m_phy, PxTransform(PxVec3((float)pos.x, (float)pos.y, (float)pos.z)), PxBoxGeometry(1.0, 1.0, 1.0), *mat, 100.0f);
		//act->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		act->userData = entity;
		m_scene->addActor(*act);
	}

	void PhysicsEngine::closeScene() {
		m_scene->release();
		m_scene = nullptr;
	}

	PhysicsEngine::~PhysicsEngine() {
		m_cooking->release();
		m_phy->release();
		if (m_pvd) {
			m_pvd->release();
			m_transport->release();
		}
		m_foundation->release();
		delete m_err;
	}
}