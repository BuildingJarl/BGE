#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "Game.h"
#include <btBulletDynamicsCommon.h>

namespace BGE 
{
	class Hand:
		public GameComponent
	{
	private:
		Leap::Controller leapmotionController;
		btDiscreteDynamicsWorld * dynamicsWorld;

		void CreatePalm();
		void CreateFingers();

	public:
		Hand(Leap::Controller leapmotionController, btDiscreteDynamicsWorld * dynamicsWorld);
		~Hand(void);

		void Update(float timeDelta);

		shared_ptr<GameComponent> palm;

		vector <shared_ptr<GameComponent>> fingers;
	};

}


