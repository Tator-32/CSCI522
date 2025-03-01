#include "PhysicsManager.h"

PE::Components::PhysicsManager& PE::Components::PhysicsManager::getInstance()
{
    static PhysicsManager instance;
    return instance;
}

void PE::Components::PhysicsManager::registerStaticCollider(Physics* collider)
{
    m_staticColliders.push_back(collider);
}

void PE::Components::PhysicsManager::checkCollisions(Physics* dynamicCollider, std::vector<Physics*> &collisions)
{
    collisions.clear();
    for(Physics* staticCollider : m_staticColliders)
	{
		if(dynamicCollider->isColliding(*staticCollider))
		{
			collisions.push_back(staticCollider);
		}
	}
}
