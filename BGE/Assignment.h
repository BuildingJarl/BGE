#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>
#include "Leap.h"
#include "LeapListener.h"

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

		PhysicsFactory * physicsFactory;
		btDiscreteDynamicsWorld * dynamicsWorld;

		LeapListener leapmotionListener;
		Leap::Controller leapmotionController;
	};
}