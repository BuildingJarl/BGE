#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "PhysicsCamera.h"

namespace BGE
{
	class FPSLeapController:public GameComponent
	{
	private:
		PhysicsController * pickedUp;
	public:
		FPSLeapController(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory);
		~FPSLeapController(void);

		void Update(float timeDelta);
		bool Initialise();

		Leap::Controller leapmotionController;
		PhysicsFactory * physicsFactory;
	};
}

