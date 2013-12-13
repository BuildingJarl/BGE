#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "Game.h"
#include <btBulletDynamicsCommon.h>
#include "PhysicsController.h"
#include "KinematicController.h"
#include "PhysicsFactory.h"
#include "ivanPatri.h"

namespace BGE 
{

	class Hand:
		public GameComponent
	{
	private:
		Leap::Controller leapmotionController;
		PhysicsFactory * physicsFactory;


	public:
		Hand(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory);
		~Hand(void);

		void Update(float timeDelta);
		bool Initialise();

		shared_ptr<ivanPatri> palm;
		shared_ptr<KinematicController> pinky;
		shared_ptr<ivanPatri> thumb;
		shared_ptr<ivanPatri> middle;

		std::vector<std::shared_ptr<ivanPatri>> fingerList;

		int gunSelection;
		float elapsedTime;
		float elapsedTimeSelection;
		float elapsedJointGun;
		float elapsedSphereGun;
		float elapsedRagDollLauncher;
		float elapsedMorphGun;
		void GunSelection(float timeDelta);
		void GravityGun();
		void JointGun(float timeDelta);
		void SphereGun(float timeDelta);
		void RagDollLauncher(float timeDelta);
		void MorphGun(float timeDelta);

		PhysicsController * pickedUpGravityGun;

	};

}


