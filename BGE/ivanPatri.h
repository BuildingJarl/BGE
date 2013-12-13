#pragma once
#include "ParticleEffect.h"

namespace BGE
{
	class ivanPatri :
		public ParticleEffect
	{
	public:
		ivanPatri(void);
		ivanPatri(int numParticles);
		~ivanPatri(void);

		bool Initialise();
		void Update(float timeDelta);
		void InitParticle(Particle & particle);
		void UpdateParticle(float timeDelta, Particle & particle);

		int numParticles;
		float elapsed;
	};
}


