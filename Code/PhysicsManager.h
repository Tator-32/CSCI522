#ifndef __PYENGINE_PHYSICSMANAGER_H__
#define __PYENGINE_PHYSICSMANAGER_H__

#include "PrimeEngine/Math/Vector3.h"
#include "Physics.h"
#include <vector>

namespace PE
{
	namespace Components
	{
		class PhysicsManager
		{
		public:
			static PhysicsManager& getInstance();
			void registerStaticCollider(Physics* collider);
			void checkCollisions(Physics* dynamicCollider, std::vector<Physics*> &collisions);

		private:
			std::vector<Physics*> m_staticColliders;
		};
	}
}
#endif
