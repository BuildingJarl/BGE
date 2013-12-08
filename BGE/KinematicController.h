#pragma once
#include "GameComponent.h"
#include "PhysicsController.h"
#include <btBulletDynamicsCommon.h>

namespace BGE

{
	class KinematicController :
		public PhysicsController, public btMotionState 
	{
	public:
		KinematicController();
		~KinematicController(void);

		//bool Initialise();
		void Update(float timeDelta);

		void getWorldTransform(btTransform &worldTrans) const;
		void setWorldTransform(const btTransform &worldTrans);
	};
}
