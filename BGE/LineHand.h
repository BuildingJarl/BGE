#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "Game.h"
#include <btBulletDynamicsCommon.h>
#include "PhysicsController.h"
#include "KinematicController.h"
#include "PhysicsFactory.h"

namespace BGE 
{
	
	struct XSmallerLH
	{
		bool operator()( const glm::vec3& lx, const glm::vec3& rx ) const {
    		return lx.x < rx.x;
		}
	};
		

	class LineHand:
		public GameComponent
	{
	private:
		Leap::Controller leapmotionController;
		btDiscreteDynamicsWorld * dynamicsWorld;


	public:
		LineHand(Leap::Controller leapmotionController,btDiscreteDynamicsWorld * dynamicsWorld);
		~LineHand(void);

		void Update(float timeDelta);
		bool Initialise();

		std::shared_ptr<PhysicsFactory> physicsFactory;

		shared_ptr<KinematicController> palm;
		shared_ptr<KinematicController> thumbTip;
		shared_ptr<KinematicController> indexTip;
		shared_ptr<KinematicController> middleTip;
		shared_ptr<KinematicController> ringTip;
		shared_ptr<KinematicController> pinkyTip;

		float elapsedLeapFrameTime;
	};

}


