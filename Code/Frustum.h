// PrimeEngine/Math/Frustum.h
#ifndef __PYENGINE_2_0_FRUSTUM_H__
#define __PYENGINE_2_0_FRUSTUM_H__

#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPU.h"

namespace PE {
    namespace Components {

        struct Plane 
        {
            Vector3 normal;
            float distance;

            void normalize() 
            {
                float mag = normal.length();
                if (mag > 0) 
                {
                    normal = normal / mag;
                    distance = distance / mag;
                }
            }
        };

        struct Frustum 
        {
            Plane planes[6];

            void ShrinkFrustum(float scaleFactor);

            Plane ComputePlane(Vector3 a, Vector3 b, Vector3 c);

            void extractPlanes(Vector3 pos, Vector3 target, Vector3 up, float verticalFov, float aspect, float nearClip, float farClip);

            bool isBoxInside(Array<PrimitiveTypes::Float32>aabbv, const Matrix4x4& worldTransform) const 
            {
                Vector3 corners[8] = {
                    worldTransform * Vector3(aabbv[0], aabbv[2], aabbv[4]),
                    worldTransform * Vector3(aabbv[1], aabbv[2], aabbv[4]),
                    worldTransform * Vector3(aabbv[0], aabbv[3], aabbv[4]),
                    worldTransform * Vector3(aabbv[1], aabbv[3], aabbv[4]),
                    worldTransform * Vector3(aabbv[0], aabbv[2], aabbv[5]),
                    worldTransform * Vector3(aabbv[1], aabbv[2], aabbv[5]),
                    worldTransform * Vector3(aabbv[0], aabbv[3], aabbv[5]),
                    worldTransform * Vector3(aabbv[1], aabbv[3], aabbv[5])
                };

                for (int i = 0; i < 6; ++i) 
                {
                    bool allOutside = true;
                    for (int j = 0; j < 8; ++j) 
                    {
                        // printf("AABB%d:%f,%f,%f\n", j, corners[j].m_x, corners[j].m_y, corners[j].m_z);
                        if (planes[i].normal.m_x * corners[j].m_x +
                            planes[i].normal.m_y * corners[j].m_y +
                            planes[i].normal.m_z * corners[j].m_z + planes[i].distance >= 0) 
                        {
                            allOutside = false;
                            break;
                        }
                    }
                    if (allOutside) return false;
                }
                return true;
            }


        };

    }; // namespace Components
}; // namespace PE
#endif