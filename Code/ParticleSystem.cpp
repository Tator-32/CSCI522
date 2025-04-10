#include "ParticleSystem.h"
#include "math.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Scene/CameraManager.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include <algorithm>

namespace PE
{
	namespace Components
	{
		PE_IMPLEMENT_CLASS1(ParticleSystem, Mesh);

		ParticleSystem::ParticleSystem(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself)
			: Mesh(context, arena, hMyself),
			m_spawnRate(8.0f),
			m_particleLifetime(3.0f),
			m_spawnPosition(Vector3(0.0f, 0.0f, 0.0f)),
			m_spawnVelocity(Vector3(0.0f, 1.0f, 0.0f)),
			m_particleSize(0.3f),
			m_loaded(false)
		{
			m_particles.reset(100);
			m_vertexData.reset(2400);

			if (!m_meshCPU.isValid())
			{
				m_meshCPU = Handle("MeshCPU ParticleMesh", sizeof(MeshCPU));
				new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
			}
		}


		void ParticleSystem::addDefaultComponents()
		{
			Mesh::addDefaultComponents();

			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_UPDATE, ParticleSystem::do_UPDATE);
			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_GATHER_DRAWCALLS, ParticleSystem::do_GATHER_DRAWCALLS);
		}

		void ParticleSystem::do_UPDATE(PE::Events::Event* pEvt)
		{
			// printf("ParticleSystem::do_UPDATE\n");
			// Update the particle system
			PE::Events::Event_UPDATE* pRealEvt = (PE::Events::Event_UPDATE*)(pEvt);
			float frameTime = pRealEvt->m_frameTime;

			// Add new particles
		    static float spawnAccumulator = 0.0f;
			spawnAccumulator += m_spawnRate * frameTime;
			int spawnCount = static_cast<int>(spawnAccumulator);
			spawnAccumulator -= spawnCount;
			for (int i = 0; i < spawnCount; ++i)
			{
				Particle p;
				p.m_position = m_spawnPosition;
				p.m_velocity = m_spawnVelocity;
				// Randomize the velocity slightly
				p.m_velocity.m_x += (rand() % 1000) / 1000.0f - 0.5f;
				p.m_velocity.m_y += (rand() % 1000) / 1000.0f - 0.5f;
				p.m_velocity.m_z += (rand() % 1000) / 1000.0f - 0.5f;
				p.m_lifetime = m_particleLifetime;
				p.m_color = Vector4(0.5f, 0.6f, 0.8f, 1.0f);

				m_particles.add(p);
			}

			// Remove expired particles
			for (int i = 0; i < m_particles.m_size;)
			{
				Particle & p = m_particles[i];
				p.m_lifetime -= frameTime;

				if (p.m_lifetime <= 0.0f)
				{
					m_particles.remove(i);
				}
				else
				{
					p.m_position += p.m_velocity * frameTime;
					// printf("ParticleSystem::do_UPDATE - Particle %d Position: %f %f %f\n", i, p.m_position.m_x, p.m_position.m_y, p.m_position.m_z);
					++i;
				}
			}
			// printf("ParticleSystem::do_UPDATE - Particle Count: %d\n", m_particles.m_size);

			updateGPUBuffer();

			// m_vertexData.clear();
		}

		void ParticleSystem::do_GATHER_DRAWCALLS(PE::Events::Event* pEvt)
		{
			//PE::Events::Event_GATHER_DRAWCALLS* pRealEvt = (PE::Events::Event_GATHER_DRAWCALLS*)(pEvt);
			//if (m_particles.m_size > 0)
			//{
			//	loadFromParticleData_needsRC(pRealEvt->m_threadOwnershipMask);
			//	printf("ParticleSystem prepared for rendering with %d particles\n", m_particles.m_size);
			//}
		}

		void ParticleSystem::updateGPUBuffer()
		{
			const int floatsPerParticle = 6;
			int particleCount = m_particles.m_size; // 3 for position, 3 for color.
			int vertexCount = particleCount * 4;

			m_vertexData.reset(vertexCount * floatsPerParticle);

			CameraSceneNode* pCam = CameraManager::Instance()->getActiveCamera()->getCamSceneNode();
			Vector3 cameraRight = pCam->getRight();
			Vector3 cameraUp = pCam->getUp();
			float halfSize = m_particleSize * 0.5f;

			for (int i = 0; i < particleCount; ++i)
			{
				Particle& p = m_particles[i];

				Vector3 offsetTL = (-cameraRight * halfSize) + (cameraUp * halfSize);
				Vector3 offsetBR = (cameraRight * halfSize) + (-cameraUp * halfSize);
				Vector3 offsetTR = (cameraRight * halfSize) + (cameraUp * halfSize);
				Vector3 offsetBL = (-cameraRight * halfSize) + (-cameraUp * halfSize);

				Vector3 posTL = p.m_position + offsetTL;
				Vector3 posTR = p.m_position + offsetTR;
				Vector3 posBR = p.m_position + offsetBR;
				Vector3 posBL = p.m_position + offsetBL;

				addVertex(m_vertexData, posTL, p.m_color);
				addVertex(m_vertexData, posTR, p.m_color);
				addVertex(m_vertexData, posBR, p.m_color);
				addVertex(m_vertexData, posBL, p.m_color);

				/*printf("ParticleSystem::TL position - (%f, %f, %f)\n",posTL.m_x ,posTL.m_y ,posTL.m_z);
				printf("ParticleSystem::TR position - (%f, %f, %f)\n",posTR.m_x ,posTR.m_y ,posTR.m_z);
				printf("ParticleSystem::BR position - (%f, %f, %f)\n",posBR.m_x ,posBR.m_y ,posBR.m_z);
				printf("ParticleSystem::BL position - (%f, %f, %f)\n",posBL.m_x ,posBL.m_y ,posBL.m_z);*/
			}
		}

		void ParticleSystem::addVertex(Array<PrimitiveTypes::Float32>& vertexData, const Vector3& pos, const Vector4& color)
		{
			vertexData.add(pos.m_x);
			vertexData.add(pos.m_y);
			vertexData.add(pos.m_z);
			vertexData.add(color.m_x);
			vertexData.add(color.m_y);
			vertexData.add(color.m_z);
		}

		void ParticleSystem::loadFromParticleData_needsRC(int& threadOwnershipMask)
		{
			int totalVertexCount = m_particles.m_size * 4;
			int totalIndexCount = m_particles.m_size * 6;

			MeshCPU& mcpu = *m_meshCPU.getObject<MeshCPU>();

			if (!m_loaded)
			{
				mcpu.createEmptyMesh();
			}

			mcpu.m_manualBufferManagement = true;

			PositionBufferCPU* pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
			ColorBufferCPU* pCB = mcpu.m_hColorBufferCPU.getObject<ColorBufferCPU>();
			IndexBufferCPU* pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();

			pIB->m_primitiveTopology = PEPrimitveTopology_TRIANGLES;
			pIB->m_verticesPerPolygon = 3;

			pVB->m_values.reset(totalVertexCount * 3);
			pCB->m_values.reset(totalVertexCount * 3);
			pIB->m_values.reset(totalIndexCount);

			if (totalVertexCount > 0)
			{
				pIB->m_indexRanges[0].m_start = 0;
				pIB->m_indexRanges[0].m_end = totalIndexCount - 1;
				pIB->m_indexRanges[0].m_minVertIndex = 0;
				pIB->m_indexRanges[0].m_maxVertIndex = totalVertexCount - 1;
				pIB->m_minVertexIndex = 0;
				pIB->m_maxVertexIndex = totalVertexCount - 1;
			}
			else
			{
				return;
			}


			for (int i = 0; i < totalVertexCount; ++i)
			{
				float x = m_vertexData[i * 6 + 0];
				float y = m_vertexData[i * 6 + 1];
				float z = m_vertexData[i * 6 + 2];

				float r = m_vertexData[i * 6 + 3];
				float g = m_vertexData[i * 6 + 4];
				float b = m_vertexData[i * 6 + 5];

				pVB->m_values.add(x);
				pVB->m_values.add(y);
				pVB->m_values.add(z);

				pCB->m_values.add(r);
				pCB->m_values.add(g);
				pCB->m_values.add(b);
				//pCB->m_values.add(1.0f); // alpha
				// printf("ParticleSystem::loadFromParticleData_needsRC - Vertex %d Position: %f %f %f Color: %f %f %f\n", i, x, y, z, r, g, b); // Debug output
			}

			for (int i = 0; i < m_particles.m_size; ++i)
			{
				int baseIndex = i * 4;
				pIB->m_values.add(baseIndex + 0);
				pIB->m_values.add(baseIndex + 1);
				pIB->m_values.add(baseIndex + 2);

				pIB->m_values.add(baseIndex + 2);
				pIB->m_values.add(baseIndex + 3);
				pIB->m_values.add(baseIndex + 0);
			}

			if (!m_loaded)
			{
				loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

				Handle hEffect = EffectManager::Instance()->getEffectHandle("ParticleTech");
				if (!hEffect.isValid())
				{
					printf("ParticleMesh::loadFromParticleData_needsRC: Effect not found");
					return;
				}
				for (int imat = 0; imat < m_effects.m_size; imat++)
				{
					if (m_effects[imat].m_size)
					{
						m_effects[imat][0] = hEffect;
					}
				}
				m_loaded = true;
			}
			else
			{
				updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
				// printf("ParticleMesh::loadFromParticleData_needsRC: Updated GPU buffers\n");
			}
		}
	}
}