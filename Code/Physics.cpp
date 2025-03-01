#include "Physics.h"
#include "math.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include <algorithm>


namespace PE
{
	namespace Components
	{
		PE_IMPLEMENT_CLASS1(Physics, Component);

		Physics::Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
			: Component(context, arena, hMyself)
		{
			m_position = Vector3(0, 0, 0);
			m_velocity = Vector3(0, 0, 0);
			m_shapeType = ShapeType::SPHERE;
			m_radius = 1.0f;
		}

		Physics::Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Vector3 position, Vector3 min, Vector3 max)
			: Component(context, arena, hMyself)
		{
			m_position = position;
			m_velocity = Vector3(0, 0, 0);
			m_shapeType = ShapeType::BOX;
			m_min = min + position;
			m_max = max + position;
		}

		Physics::Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Vector3 position, float radius)
			: Component(context, arena, hMyself)
		{
			m_position = position;
			m_velocity = Vector3(0, 0, 0);
			m_shapeType = ShapeType::SPHERE;
			m_radius = radius;
		}

		void Physics::update(float deltaTime) // seems not to be used
		{
			m_position += m_velocity * deltaTime;
		}

		void Physics::setVelocity(const Vector3& velocity)
		{
			m_velocity = velocity;
		}

		void Physics::setPosition(const Vector3& position)
		{
			m_position = position;
		}

		bool Physics::isColliding(const Physics& other)
		{
			// We only process collisions between the car and the soldier
			if (other.m_name != "nazicar.x_carmesh_mesh.mesha" && other.m_name != "cobbleplane.x_pplaneshape1_mesh.mesha")
			{
				return false;
			}
			if ((m_shapeType == ShapeType::SPHERE && other.m_shapeType == ShapeType::BOX) ||
				(m_shapeType == ShapeType::BOX && other.m_shapeType == ShapeType::SPHERE))
			{
				const Physics* sphere = (m_shapeType == ShapeType::SPHERE) ? this : &other;
				const Physics* box = (m_shapeType == ShapeType::SPHERE) ? &other : this;

				Vector3 sphereCenter = sphere->m_position;
				float radius = sphere->m_radius;

				Vector3 closestPoint(
					std::max(box->m_min.getX(), std::min(sphereCenter.getX(), box->m_max.getX())),
					std::max(box->m_min.getY(), std::min(sphereCenter.getY(), box->m_max.getY())),
					std::max(box->m_min.getZ(), std::min(sphereCenter.getZ(), box->m_max.getZ()))
				);

				float distanceSquared = (closestPoint - sphereCenter).lengthSqr();
				if (distanceSquared < (radius * radius) && other.m_name == "cobbleplane.x_pplaneshape1_mesh.mesha")
				{
					hitGround = true;
				}
				else if (distanceSquared < (radius * radius) && other.m_name == "nazicar.x_carmesh_mesh.mesha")
				{
					hitCar = true;
				}
				return distanceSquared < (radius * radius);
			}
			return false;
		}

		Vector3 Physics::resolveCollision(const Physics& other, float speed, float frameTime)
		{
			if (other.m_name == "cobbleplane.x_pplaneshape1_mesh.mesha")
			{
				return m_position;
			}
			Vector3 closestPoint(
				std::max(other.m_min.getX(), std::min(m_position.getX(), other.m_max.getX())),
				std::max(other.m_min.getY(), std::min(m_position.getY(), other.m_max.getY())),
				std::max(other.m_min.getZ(), std::min(m_position.getZ(), other.m_max.getZ()))
			);

			Vector3 normal = m_position - closestPoint;
			normal.normalize();
			normal.m_y = 0;

			Vector3 velocity = m_velocity;
			Vector3 slideVelocity = velocity - normal * (velocity.dotProduct(normal));

			slideVelocity.normalize();
			slideVelocity *= speed;

			Vector3 newPosition = m_position + slideVelocity * frameTime;

			if (newPosition.getX() >= other.m_min.getX() && newPosition.getX() <= other.m_max.getX() &&
				newPosition.getZ() >= other.m_min.getZ() && newPosition.getZ() <= other.m_max.getZ())
			{
				newPosition = closestPoint + normal * 0.01f;
			}

			return newPosition;
		}

		Vector3 Physics::applyGravity(float frameTime)
		{
			Vector3 newPosition = m_position;

			newPosition -= Vector3(0, 1.0, 0) * frameTime;

			return newPosition;
		}

	}
}