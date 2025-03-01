#ifndef __PYENGINE_2_0_PHYSICS_H__
#define __PYENGINE_2_0_PHYSICS_H__

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"

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

namespace PE
{
	namespace Components
	{
		struct Physics : public Component
		{
			PE_DECLARE_CLASS(Physics);

			enum ShapeType
			{
				SPHERE,
				BOX
			};

			Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);

			Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Vector3 position, Vector3 min, Vector3 max);

			Physics(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Vector3 position, float radius);

			void update(float deltaTime);
			void setVelocity(const Vector3& velocity);
			Vector3 getVelocity() const { return m_velocity; };

			void setPosition(const Vector3& position);
			Vector3 getPosition() const { return m_position; };

			ShapeType getShapeType() const { return m_shapeType; };

			bool isColliding(const Physics& other);

			Vector3 resolveCollision(const Physics& other, float speed, float frameTime);

			Vector3 applyGravity(float frameTime);

			Vector3 m_position;
			Vector3 m_velocity;
			ShapeType m_shapeType;
			Vector3 m_min;
			Vector3 m_max;
			float m_radius;
			std::string m_name;
			bool useGravity = false;
			bool hitGround = false;
			bool hitCar = false;
		};
	};
};

#endif // __PYENGINE_2_0_PHYSICS_H__
