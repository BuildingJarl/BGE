#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"

using namespace std;

namespace BGE
{

	class Player :
		public GameComponent
	{
	public:
		Player(btDiscreteDynamicsWorld * dynamicsWorld);
		~Player(void);

		void Update(float timeDelta);

		btDiscreteDynamicsWorld * dynamicsWorld;
	};

}