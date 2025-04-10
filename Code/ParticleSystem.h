#ifndef __PYENGINE_2_0_PARTICLE_SYSTEM_H__
#define __PYENGINE_2_0_PARTICLE_SYSTEM_H__
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <iostream>
#include <string>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "ParticleMesh.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Mesh.h"


namespace PE
{
	namespace Components
	{
		struct Particle
		{
			Vector3 m_position;
			Vector3 m_velocity;
			float m_lifetime;
			Vector4 m_color;
		};

		struct ParticleSystem : public Mesh
		{
			PE_DECLARE_CLASS(ParticleSystem);

			ParticleSystem(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself);
			virtual void addDefaultComponents();

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE)
			virtual void do_UPDATE(PE::Events::Event* pEvt);
			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS)
			virtual void do_GATHER_DRAWCALLS(PE::Events::Event* pEvt);

			void updateGPUBuffer();
			void addVertex(Array<PrimitiveTypes::Float32>& vertexData, const Vector3& pos, const Vector4& color);
			void loadFromParticleData_needsRC(int& threadOwnershipMask);
			

			float m_spawnRate;
			float m_particleLifetime; 
			Vector3 m_spawnPosition;
			Vector3 m_spawnVelocity;
			float m_particleSize;

			Array<Particle> m_particles;
			Array<PrimitiveTypes::Float32> m_vertexData;

			Handle m_meshCPU;
			bool m_loaded;
		};
	}
}
#endif // __PYENGINE_2_0_PARTICLE_SYSTEM_H__
