#pragma once
#include "PhysicsController.h"
#include <btBulletDynamicsCommon.h>
#include "Utils.h"


namespace BGE
{
	class PhysicsHand:
		public PhysicsController, public btMotionState
	{
	public:
		PhysicsHand();
		~PhysicsHand(void);

		void PhysicsHand::Update(float timeDelta);
		void getWorldTransform(btTransform &worldTrans) const;
		void setWorldTransform(const btTransform &worldTrans);
	};

}



