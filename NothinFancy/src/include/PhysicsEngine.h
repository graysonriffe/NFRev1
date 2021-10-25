#pragma once
#include <PxConfig.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace nf {
	class Application;
	class Entity;

	class PhysicsEngine {
	public:
		PhysicsEngine(Application* app);

		void newScene();
		void update(double dt);
		void addActor(Entity* entity);
		void closeScene();

		~PhysicsEngine();
	private:
		Application* m_app;
		PxFoundation* m_foundation;
		PxPvd* m_pvd;
		PxPvdTransport* m_transport;
		PxPhysics* m_phy;
		PxCooking* m_cooking;
		PxCpuDispatcher* m_dispacher;
		PxScene* m_scene;

		PxDefaultAllocator m_alloc;
		PxErrorCallback* m_err;

		const double m_stepSize;

		double m_accumulator;
	};
}