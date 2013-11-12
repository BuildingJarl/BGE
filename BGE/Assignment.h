#pragma once
#include "Game.h"
#include "GameComponent.h"

using namespace std;

namespace BGE
{
	class Assignment :
		public Game
	{
	public:
		Assignment(void);
		~Assignment(void);
		
		
		bool Initialise();
		void Update(float timeDelta);
	};
}