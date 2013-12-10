#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "PhysicsCamera.h"
#include "KinematicController.h"

namespace BGE
{
	class FPSLeapController:public GameComponent
	{
	private:
		PhysicsController * pickedUp;
		bool triggerPulled;
		int gunSelection;
		float elapsed;

		void GravityGun(Leap::Finger thumbNew, Leap::Finger indexNew);
		void JointGun(Leap::Finger thumbNew, Leap::Finger indexNew);
		void SphereGun(Leap::Finger thumbNew, Leap::Finger indexNew, float timeDelta);
		void RagDollGun(Leap::Finger thumbNew, Leap::Finger indexNew, float timeDelta);
		void MorphGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta);
		void PartiGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta);

		shared_ptr<KinematicController> jointGunSphere;
	public:
		FPSLeapController(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory);
		~FPSLeapController(void);

		void Update(float timeDelta);
		bool Initialise();

		Leap::Controller leapmotionController;
		PhysicsFactory * physicsFactory;
	};
}

