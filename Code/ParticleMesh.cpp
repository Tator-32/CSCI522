#include "ParticleMesh.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Geometry/MeshCPU/MeshCPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/Light.h"
#include "PrimeEngine/GameObjectModel/Camera.h"

// Sibling/Children includes
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Scene/CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"

namespace PE
{
	namespace Components
	{
		PE_IMPLEMENT_CLASS1(ParticleMesh, Mesh);

		void ParticleMesh::loadFromParticleData_needsRC(const float* vals, int numParticles, const char* techName, int& threadOwnershipMask)
		{
			int totalVertexCount = numParticles * 4;
			int totalIndexCount = numParticles * 6;

			if (!m_meshCPU.isValid())
			{
				m_meshCPU = Handle("MeshCPU ParticleMesh", sizeof(MeshCPU));
				new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
			}
			MeshCPU &mcpu = *m_meshCPU.getObject<MeshCPU>();

			if (!m_loaded)
			{
				mcpu.createEmptyMesh();
			}

			mcpu.m_manualBufferManagement = true;

			PositionBufferCPU *pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
			ColorBufferCPU *pCB = mcpu.m_hColorBufferCPU.getObject<ColorBufferCPU>();
			IndexBufferCPU *pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();

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
				float x = vals[i * 6 + 0];
				float y = vals[i * 6 + 1];
				float z = vals[i * 6 + 2];

				float r = vals[i * 6 + 3];
				float g = vals[i * 6 + 4];
				float b = vals[i * 6 + 5];

				pVB->m_values.add(x);
				pVB->m_values.add(y);
				pVB->m_values.add(z);

				pCB->m_values.add(r);
				pCB->m_values.add(g);
				pCB->m_values.add(b);
				//pCB->m_values.add(1.0f); // alpha
			}

			for (int i = 0; i < numParticles; ++i)
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

				Handle hEffect = EffectManager::Instance()->getEffectHandle(techName);
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
			}
		}
	}
}

