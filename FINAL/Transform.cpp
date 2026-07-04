#include "pch.h"
#include "Transform.h"
#include <cmath>

#define PI 3.14159265358979323846f

CTransform::CTransform()
{
    SetIdentity();
}

void CTransform::SetIdentity()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
}

void CTransform::Multiply(const CTransform& other)
{
    float result[4][4] = {};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                result[i][j] += m[i][k] * other.m[k][j];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = result[i][j];
}

CTransform CTransform::Translation(float tx, float ty, float tz)
{
    CTransform t;
    t.m[0][3] = tx;
    t.m[1][3] = ty;
    t.m[2][3] = tz;
    return t;
}

CTransform CTransform::RotationX(float angleDeg)
{
    CTransform r;
    float rad = angleDeg * PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    r.m[1][1] = c;  r.m[1][2] = -s;
    r.m[2][1] = s;  r.m[2][2] = c;
    return r;
}

CTransform CTransform::RotationY(float angleDeg)
{
    CTransform r;
    float rad = angleDeg * PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    r.m[0][0] = c;  r.m[0][2] = s;
    r.m[2][0] = -s; r.m[2][2] = c;
    return r;
}

CTransform CTransform::RotationZ(float angleDeg)
{
    CTransform r;
    float rad = angleDeg * PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    r.m[0][0] = c;  r.m[0][1] = -s;
    r.m[1][0] = s;  r.m[1][1] = c;
    return r;
}

CTransform CTransform::Scaling(float sx, float sy, float sz)
{
    CTransform s;
    s.m[0][0] = sx;
    s.m[1][1] = sy;
    s.m[2][2] = sz;
    return s;
}

CTransform CTransform::LookAt(float eyeX, float eyeY, float eyeZ,
    float targetX, float targetY, float targetZ,
    float upX, float upY, float upZ)
{
    float fx = targetX - eyeX, fy = targetY - eyeY, fz = targetZ - eyeZ;
    float flen = sqrtf(fx * fx + fy * fy + fz * fz);
    fx /= flen; fy /= flen; fz /= flen;

    float ux = upX, uy = upY, uz = upZ;
    float ulen = sqrtf(ux * ux + uy * uy + uz * uz);
    ux /= ulen; uy /= ulen; uz /= ulen;

    float sx = fy * uz - fz * uy;
    float sy = fz * ux - fx * uz;
    float sz = fx * uy - fy * ux;

    float uCrossX = sy * fz - sz * fy;
    float uCrossY = sz * fx - sx * fz;
    float uCrossZ = sx * fy - sy * fx;

    CTransform v;
    v.m[0][0] = sx;  v.m[0][1] = sy;  v.m[0][2] = sz;
    v.m[1][0] = uCrossX; v.m[1][1] = uCrossY; v.m[1][2] = uCrossZ;
    v.m[2][0] = -fx; v.m[2][1] = -fy; v.m[2][2] = -fz;
    v.m[0][3] = -(sx * eyeX + sy * eyeY + sz * eyeZ);
    v.m[1][3] = -(uCrossX * eyeX + uCrossY * eyeY + uCrossZ * eyeZ);
    v.m[2][3] = (fx * eyeX + fy * eyeY + fz * eyeZ);
    return v;
}

CTransform CTransform::PerspectiveFovRH(float fovYDeg, float aspect,
    float nearZ, float farZ)
{
    CTransform p;
    float rad = fovYDeg * PI / 180.0f;
    float h = 1.0f / tanf(rad * 0.5f);
    float w = h / aspect;
    p.SetIdentity();
    p.m[0][0] = w;
    p.m[1][1] = h;
    p.m[2][2] = farZ / (nearZ - farZ);
    p.m[2][3] = (nearZ * farZ) / (nearZ - farZ);
    p.m[3][2] = -1.0f;
    p.m[3][3] = 0.0f;
    return p;
}

void CTransform::TransformPoint(float x, float y, float z,
    float& outX, float& outY, float& outZ, float& outW) const
{
    outX = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3];
    outY = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3];
    outZ = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3];
    outW = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3];
}

void CTransform::TransformDirection(float x, float y, float z,
    float& outX, float& outY, float& outZ) const
{
    outX = m[0][0] * x + m[0][1] * y + m[0][2] * z;
    outY = m[1][0] * x + m[1][1] * y + m[1][2] * z;
    outZ = m[2][0] * x + m[2][1] * y + m[2][2] * z;
}
