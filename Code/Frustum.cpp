#include "Frustum.h"

namespace PE {
    namespace Components {

        Plane Frustum::ComputePlane(Vector3 a, Vector3 b, Vector3 c)
        {
            Vector3 normal = (c - a).crossProduct(b - a);
            float d = -normal * a;
            Plane res;
            res.normal = normal;
            res.distance = d;
            res.normalize();
            return res;
        }

        void Frustum::ShrinkFrustum(float scaleFactor)
        {
            for (int i = 0; i < 6; i++) 
            {
                float shrinkFactor = 1.0f - scaleFactor;
                Vector3 pointOnPlane = -planes[i].normal * planes[i].distance;
                pointOnPlane += planes[i].normal * (planes[i].distance * shrinkFactor);
                planes[i].distance = -planes[i].normal.dotProduct(pointOnPlane);
            }
        }


        void Frustum::extractPlanes(Vector3 pos, Vector3 target, Vector3 up, float verticalFov, float aspect, float nearClip, float farClip)
        {
            Vector3 forward = (target - pos);
            forward.normalize();
            Vector3 right = up.crossProduct(forward);
            right.normalize();
            Vector3 upNew = forward.crossProduct(right);
            upNew.normalize();

            Vector3 nearCenter = pos + forward * nearClip;
            Vector3 farCenter = pos + forward * farClip * 0.8f;

            float tanFov = tan(verticalFov * 0.65f / 2.0f);
            float nearHeight = 2.0f * nearClip * tanFov;
            float nearWidth = nearHeight * aspect;
            float farHeight = 2.0f * farClip * tanFov;
            float farWidth = farHeight * aspect;

            Vector3 nearTopLeft = nearCenter + upNew * (nearHeight / 2) - right * (nearWidth / 2);
            // printf("Corner1:%f,%f,%f\n", nearTopLeft.m_x, nearTopLeft.m_y, nearTopLeft.m_z);
            Vector3 nearTopRight = nearCenter + upNew * (nearHeight / 2) + right * (nearWidth / 2);
            // printf("Corner2:%f,%f,%f\n", nearTopRight.m_x, nearTopRight.m_y, nearTopRight.m_z);
            Vector3 nearBottomLeft = nearCenter - upNew * (nearHeight / 2) - right * (nearWidth / 2);
            // printf("Corner3:%f,%f,%f\n", nearBottomLeft.m_x, nearBottomLeft.m_y, nearBottomLeft.m_z);
            Vector3 nearBottomRight = nearCenter - upNew * (nearHeight / 2) + right * (nearWidth / 2);
            // printf("Corner4:%f,%f,%f\n", nearBottomRight.m_x, nearBottomRight.m_y, nearBottomRight.m_z);
            Vector3 farTopLeft = farCenter + upNew * (farHeight / 2) - right * (farWidth / 2);
            // printf("Corner5:%f,%f,%f\n", farTopLeft.m_x, farTopLeft.m_y, farTopLeft.m_z);
            Vector3 farTopRight = farCenter + upNew * (farHeight / 2) + right * (farWidth / 2);
            // printf("Corner6:%f,%f,%f\n", farTopRight.m_x, farTopRight.m_y, farTopRight.m_z);
            Vector3 farBottomLeft = farCenter - upNew * (farHeight / 2) - right * (farWidth / 2);
            // printf("Corner7:%f,%f,%f\n", farBottomLeft.m_x, farBottomLeft.m_y, farBottomLeft.m_z);
            Vector3 farBottomRight = farCenter - upNew * (farHeight / 2) + right * (farWidth / 2);
            // printf("Corner8:%f,%f,%f\n", farBottomRight.m_x, farBottomRight.m_y, farBottomRight.m_z);


            planes[0] = ComputePlane(nearTopLeft, nearTopRight, nearBottomRight);
            planes[1] = ComputePlane(farTopRight, farTopLeft, farBottomLeft);
            planes[2] = ComputePlane(nearTopLeft, nearBottomLeft, farBottomLeft);
            planes[3] = ComputePlane(nearBottomRight, nearTopRight, farBottomRight);
            planes[4] = ComputePlane(nearTopRight, nearTopLeft, farTopLeft);
            planes[5] = ComputePlane(nearBottomLeft, nearBottomRight, farBottomRight);


            //for (int i = 0; i < 6; i++)
            //{
            //    printf("Plane%d:%f,%f,%f,%f\n",
            //        i, planes[i].normal.m_x, planes[i].normal.m_y, planes[i].normal.m_z, planes[i].distance);
            //}
        }


    }; // namespace Components
}; // namespace PE