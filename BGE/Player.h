#pragma once
#include "GameComponent.h"

using namespace std;

namespace BGE
{

	class Player :
		public GameComponent
	{
	public:
		Player(void);
		~Player(void);
	};

}