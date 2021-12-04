#include "nf/PhysicsEngine.h"

#include "nf/Application.h"
#include "nf/Entity.h"
#include "nf/Model.h"

namespace nf {
	class PhysicsErrorCallback : public PxErrorCallback {
		virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
#ifdef _DEBUG
			Debug::ErrorImp(message, file, line);
			__debugbreak();
#else
			NFError(message);
#endif
		}
	};

	PhysicsEngine::PhysicsEngine(Application* app) :
		m_app(app),
		m_err(nullptr),
		m_foundation(nullptr),
		m_pvd(nullptr),
		m_transport(nullptr),
		m_phy(nullptr),
		m_cooking(nullptr),
		m_dispacher(nullptr),
		m_defaultMat(nullptr),
		m_scene(nullptr),
		m_stepSize(1.0f / 60.0f),
		m_accumulator(0.0)
	{
		NFLog("Initializing physics engine...");
		m_err = new PhysicsErrorCallback;
		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_alloc, *m_err);
		if (!m_foundation)
			NFError("Could not initialize physics engine!");

#ifdef _DEBUG
		m_pvd = PxCreatePvd(*m_foundation);
		m_transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		m_pvd->connect(*m_transport, PxPvdInstrumentationFlag::eALL);
#endif

		m_phy = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), false, m_pvd);
		if (!m_phy)
			NFError("Could not initialize physics engine!");

		m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(PxTolerancesScale()));
		if (!m_cooking)
			NFError("Could not initialize physics engine!");

		unsigned int threads = std::thread::hardware_concurrency() - 1;
		if (threads < 0) threads = 0;
		m_dispacher = PxDefaultCpuDispatcherCreate(threads);

		m_defaultMat = m_phy->createMaterial(0.5f, 0.5f, 0.0f);

		NFLog("Initialized physics engine");
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

		//Ground plane at the very bottom (Is this a good position?)
		PxRigidStatic* ground = PxCreatePlane(*m_phy, PxPlane(0, 1, 0, 200), *m_defaultMat);
		m_scene->addActor(*ground);
	}

	void PhysicsEngine::setGravity(const Vec3& grav) {
		if (m_scene) {
			m_scene->setGravity(PxVec3(grav.x, grav.y, grav.z));

			unsigned int count = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
			PxActor** actors = new PxActor*[count];
			m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, count);

			for (unsigned int i = 0; i < count; i++) {
				if (((PxRigidDynamic*)actors[i])->isSleeping())
					((PxRigidDynamic*)actors[i])->wakeUp();
			}
			delete[] actors;
		}
	}

	void PhysicsEngine::setActorVelocity(Entity* ent, const Vec3& vel) {
		unsigned int count = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
		PxActor** actors = new PxActor*[count];
		m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, count);

		for (unsigned int i = 0; i < count; i++) {
			Entity* curr = (Entity*)actors[i]->userData;
			if (curr == ent) {
				((PxRigidDynamic*)actors[i])->setLinearVelocity(PxVec3(vel.x, vel.y, vel.z), true);
				break;
			}
		}
		delete[] actors;
	}

	void PhysicsEngine::setActorMass(Entity* ent, float mass) {
		unsigned int count = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
		PxActor** actors = new PxActor*[count];
		m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, count);

		for (unsigned int i = 0; i < count; i++) {
			Entity* curr = (Entity*)actors[i]->userData;
			if (curr == ent) {
				((PxRigidDynamic*)actors[i])->setMass(mass);
				break;
			}
		}
		delete[] actors;
	}

	void PhysicsEngine::update(float dt) {
		if (!m_scene || !m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC)) return;

		unsigned int count = m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		PxActor** actors = new PxActor*[count];
		m_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors, count);

		//Starting at 1 since 0 is the ground plane
		for (unsigned int i = 1; i < count; i++) {
			Entity* currEnt = (Entity*)actors[i]->userData;
			if (!currEnt->needsPhysicsUpdate()) continue;

			Vec3 posTemp = currEnt->getPosition();
			PxVec3 pos(posTemp.x, posTemp.y, posTemp.z);
			Vec4 rotTemp = currEnt->getRotation();
			PxQuat rot(rotTemp.x, rotTemp.y, rotTemp.z, rotTemp.w);
			PxTransform t(pos, rot);
			Vec3 scaleTemp = currEnt->getScale();
			PxVec3 scaleTemp2(scaleTemp.x, scaleTemp.y, scaleTemp.z);
			PxMeshScale scale(scaleTemp2);

			if (currEnt->getType() == Entity::Type::DYNAMIC) {
				updateEnt<PxRigidDynamic>(actors[i], t, scale);
			}
			else
				updateEnt<PxRigidStatic>(actors[i], t, scale);
		}
		delete[] actors;

		m_accumulator += dt;
		unsigned int stepCount = (unsigned int)(m_accumulator / m_stepSize);
		m_accumulator -= stepCount * m_stepSize;
		for (unsigned int i = 0; i < stepCount; i++) {
			m_scene->simulate(m_stepSize);
			m_scene->fetchResults(true);
		}

		PxActor** actives = m_scene->getActiveActors(count);
		for (unsigned int i = 0; i < count; i++) {
			Entity& ent = *(Entity*)actives[i]->userData;
			PxTransform transform = ((PxRigidActor*)actives[i])->getGlobalPose();
			ent.setPositionPhysics(Vec3(transform.p.x, transform.p.y, transform.p.z));
			ent.setRotationPhysics(Vec4(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
		}
	}

	void PhysicsEngine::addConvexMesh(Model* model, std::vector<float>& vertices) {
		PxConvexMeshDesc desc;
		desc.points.count = (unsigned int)vertices.size() / 3;
		desc.points.stride = 3 * sizeof(float);
		desc.points.data = &vertices[0];
		desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxDefaultMemoryOutputStream buf;
		if (!m_cooking->cookConvexMesh(desc, buf))
			NFError("Could not create convex mesh!");

		PxDefaultMemoryInputData in(buf.getData(), buf.getSize());
		PxConvexMesh* mesh = m_phy->createConvexMesh(in);
		m_convexMeshes[model] = mesh;
	}

	void PhysicsEngine::addTriangleMesh(Model* model, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
		PxTriangleMeshDesc desc;
		desc.points.count = (unsigned int)vertices.size() / 3;
		desc.points.stride = sizeof(float) * 3;
		desc.points.data = &vertices[0];
		desc.triangles.count = (unsigned int)indices.size() / 3;
		desc.triangles.stride = sizeof(unsigned int) * 3;
		desc.triangles.data = &indices[0];

		PxDefaultMemoryOutputStream buf;
		if (!m_cooking->cookTriangleMesh(desc, buf))
			NFError("Could not create triangle mesh!");

		PxDefaultMemoryInputData in(buf.getData(), buf.getSize());
		PxTriangleMesh* mesh = m_phy->createTriangleMesh(in);
		m_triangleMeshes[model] = mesh;
	}

	void PhysicsEngine::addActor(Entity* entity) {
		Entity::Type type = entity->getType();

		//TODO: Materials system for physics; Default material if none provided
		PxMaterial* mat;
		mat = m_defaultMat;
		float density = 100.0f;

		//Get mesh
		PxConvexMesh* convexMesh = nullptr;
		PxTriangleMesh* triangleMesh = nullptr;
		if (m_convexMeshes.find(entity->getModel()) != m_convexMeshes.end())
			convexMesh = m_convexMeshes[entity->getModel()];
		else if (m_triangleMeshes.find(entity->getModel()) != m_triangleMeshes.end())
			triangleMesh = m_triangleMeshes[entity->getModel()];
		else
			NFError("No physics mesh found for this entity!");

		//Dynamic or static
		if (type == Entity::Type::DYNAMIC) {
			PxRigidDynamic* act = PxCreateDynamic(*m_phy, PxTransform(PxIdentity), PxConvexMeshGeometry(convexMesh), *mat, density);
			act->userData = entity;
			m_scene->addActor(*act);
		}
		else if (type == Entity::Type::STATIC) {
			PxRigidStatic* act = PxCreateStatic(*m_phy, PxTransform(PxIdentity), PxConvexMeshGeometry(convexMesh), *mat);
			act->userData = entity;
			m_scene->addActor(*act);
		}
		else if (type == Entity::Type::MAP) {
			PxRigidStatic* act = PxCreateStatic(*m_phy, PxTransform(PxIdentity), PxTriangleMeshGeometry(triangleMesh), *mat);
			act->userData = entity;
			m_scene->addActor(*act);
		}
	}

	void PhysicsEngine::closeScene() {
		if (m_scene) {
			//Does this actually release all of them? Only if the respective shapes have been released?
			for (auto it = m_convexMeshes.begin(); it != m_convexMeshes.end();) {
				if (!it->first->isBaseAsset()) {
					it->second->release();
					it = m_convexMeshes.erase(it);
				}
				else
					it++;
			}
			for (auto it = m_triangleMeshes.begin(); it != m_triangleMeshes.end();) {
				if (!it->first->isBaseAsset()) {
					it->second->release();
					it = m_triangleMeshes.erase(it);
				}
				else
					it++;
			}
			m_scene->release();
			m_scene = nullptr;
		}
	}

	template<typename ActorType>
	void PhysicsEngine::updateEnt(PxActor* act, PxTransform& transform, PxMeshScale& scale) {
		((ActorType*)act)->setGlobalPose(transform);
		PxShape** shape = new PxShape*;
		((ActorType*)act)->getShapes(shape, sizeof(shape));
		PxGeometryHolder holder = shape[0]->getGeometry();
		holder.convexMesh().scale = scale;
		shape[0]->setGeometry(holder.convexMesh());
		delete shape;
	}

	PhysicsEngine::~PhysicsEngine() {
		closeScene();
		m_dispacher->release();
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