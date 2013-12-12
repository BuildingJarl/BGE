#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "Game.h"
#include "Utils.h"
#include "KinematicController.h"
#include "PhysicsFactory.h"

namespace BGE 
{
	class LeapHandGun:
		public GameComponent
	{
	public:
		LeapHandGun(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory);
		~LeapHandGun(void);

		Leap::Controller leapmotionController;
		shared_ptr<KinematicController> palm;
		shared_ptr<KinematicController> thumb;
		shared_ptr<KinematicController> index;
		shared_ptr<KinematicController> middle;
		shared_ptr<KinematicController> ring;
		shared_ptr<KinematicController> pinky;

		PhysicsFactory * physicsFactory;

		int gunSelection;
		float elapsedTime;
		float elapsedTimeSelection;
		float elapsedJointGun;
		float elapsedSphereGun;
		float elapsedRagDollLauncher;
		float elapsedMorphGun;

		void Update(float timeDelta);
		bool Initialise();
		void CalcPositions();
		void GunSelection(float timeDelta);

		void GravityGun();
		void JointGun(float timeDelta);
		void SphereGun(float timeDelta);
		void RagDollLauncher(float timeDelta);
		void MorphGun(float timeDelta);

		PhysicsController * pickedUpGravityGun;
	};
}
