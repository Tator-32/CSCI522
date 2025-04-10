#ifndef __PYENGINE_2_0_PARTICLE_MESH_H__
#define __PYENGINE_2_0_PARTICLE_MESH_H__

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include <assert.h>
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/Scene/Mesh.h"


namespace PE
{
	namespace Components
	{
		struct ParticleMesh : public Mesh
		{
			PE_DECLARE_CLASS(ParticleMesh);

			ParticleMesh(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself) : Mesh(context, arena, hMyself)
			{
				Mesh::addDefaultComponents();
				m_loaded = false;
			}

			void loadFromParticleData_needsRC(const float* vals, int numParticles, const char* techName, int& threadOwnershipMask);
			Handle m_meshCPU;
			bool m_loaded;
		};
	}
}

#endif // __PYENGINE_2_0_PARTICLE_MESH_H__