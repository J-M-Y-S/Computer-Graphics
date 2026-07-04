#pragma once
#include "Transform.h"

class CProjection
{
public:
    CProjection();

    void Setup(int viewportW, int viewportH, float fovYDeg, float nearZ, float farZ);

    CTransform BuildWorldMatrix(float rotX, float rotY, float rotZ,
        float tx, float ty, float tz, float scale) const;

    CTransform BuildViewMatrix(float camDist) const;

    bool Project(const CTransform& world, float vx, float vy, float vz,
        float& sx, float& sy, float& sz) const;

    int GetViewportW() const { return m_vpW; }
    int GetViewportH() const { return m_vpH; }

private:
    void ViewportTransform(float ndcX, float ndcY, float ndcZ,
        float& screenX, float& screenY, float& screenZ) const;

    int m_vpW, m_vpH;
    float m_fovY, m_nearZ, m_farZ;
};
