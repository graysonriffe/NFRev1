#pragma once
#include <unordered_map>
#include <PxConfig.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace nf {
	class Application;
	class Entity;
	class Model;

	class PhysicsEngine {
	public:
		PhysicsEngine(Application* app);

		void newScene();
		void update(float dt);
		void addMesh(Model* model, std::vector<float>& vertices);
		void addActor(Entity* entity);
		void closeScene();

		~PhysicsEngine();
	private:
		template<typename ActorType>
		void updateEnt(PxActor* act, PxTransform& transform, PxMeshScale& scale);

		Application* m_app;
		PxDefaultAllocator m_alloc;
		PxErrorCallback* m_err;
		PxFoundation* m_foundation;
		PxPvd* m_pvd;
		PxPvdTransport* m_transport;
		PxPhysics* m_phy;
		PxCooking* m_cooking;
		PxDefaultCpuDispatcher* m_dispacher;
		PxMaterial* m_defaultMat;
		PxScene* m_scene;
		std::unordered_map<Model*, PxConvexMesh*> m_meshes;

		const float m_stepSize;
		float m_accumulator;
	};
}