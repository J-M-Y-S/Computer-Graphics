#include "pch.h"
#include "Projection.h"
#include <cmath>

#define PI 3.14159265358979323846f

CProjection::CProjection()
    : m_vpW(800), m_vpH(600), m_fovY(60.0f), m_nearZ(0.1f), m_farZ(100.0f)
{
}

void CProjection::Setup(int viewportW, int viewportH, float fovYDeg, float nearZ, float farZ)
{
    m_vpW = viewportW;
    m_vpH = viewportH;
    m_fovY = fovYDeg;
    m_nearZ = nearZ;
    m_farZ = farZ;
}

CTransform CProjection::BuildWorldMatrix(float rotX, float rotY, float rotZ,
    float tx, float ty, float tz, float scale) const
{
    CTransform world;
    CTransform rx = CTransform::RotationX(rotX);
    CTransform ry = CTransform::RotationY(rotY);
    CTransform rz = CTransform::RotationZ(rotZ);
    CTransform sc = CTransform::Scaling(scale, scale, scale);
    CTransform tr = CTransform::Translation(tx, ty, tz);

    world = sc;
    world.Multiply(rx);
    world.Multiply(ry);
    world.Multiply(rz);
    world.Multiply(tr);
    return world;
}

CTransform CProjection::BuildViewMatrix(float camDist) const
{
    return CTransform::LookAt(0, 0, camDist, 0, 0, 0, 0, 1, 0);
}

void CProjection::ViewportTransform(float ndcX, float ndcY, float ndcZ,
    float& screenX, float& screenY, float& screenZ) const
{
    screenX = (ndcX + 1.0f) * 0.5f * m_vpW;
    screenY = (1.0f - ndcY) * 0.5f * m_vpH;
    screenZ = ndcZ;
}

bool CProjection::Project(const CTransform& world, float vx, float vy, float vz,
    float& sx, float& sy, float& sz) const
{
    float aspect = (float)m_vpW / (float)m_vpH;
    CTransform proj = CTransform::PerspectiveFovRH(m_fovY, aspect, m_nearZ, m_farZ);
    CTransform view = BuildViewMatrix(2.5f);

    CTransform mvp = world;
    mvp.Multiply(view);
    mvp.Multiply(proj);

    float x, y, z, w;
    mvp.TransformPoint(vx, vy, vz, x, y, z, w);

    if (w <= 1e-10f) return false;

    float ndcX = x / w;
    float ndcY = y / w;
    float ndcZ = z / w;

    ViewportTransform(ndcX, ndcY, ndcZ, sx, sy, sz);
    return true;
}
