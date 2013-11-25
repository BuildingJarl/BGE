#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>

using namespace std;

namespace BGE
{
	class Assignment :
		public Game
	{
	private:
		btBroadphaseInterface* broadphase;
		btDefaultCollisionConfiguration * collisionConfiguration;
		btCollisionDispatcher * dispatcher;
		btSequentialImpulseConstraintSolver * solver;

	public:
		Assignment(void);
		~Assignment(void);
		
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();

		// The world.
		std::shared_ptr<PhysicsFactory> physicsFactory;
		btDiscreteDynamicsWorld * dynamicsWorld;
		
		shared_ptr<GameComponent> leapMotionHand;
	};
}