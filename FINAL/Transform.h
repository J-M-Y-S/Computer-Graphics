#pragma once

class CTransform
{
public:
    float m[4][4];

    CTransform();
    void SetIdentity();
    void Multiply(const CTransform& other);

    static CTransform Translation(float tx, float ty, float tz);
    static CTransform RotationX(float angleDeg);
    static CTransform RotationY(float angleDeg);
    static CTransform RotationZ(float angleDeg);
    static CTransform Scaling(float sx, float sy, float sz);

    static CTransform LookAt(float eyeX, float eyeY, float eyeZ,
        float targetX, float targetY, float targetZ,
        float upX, float upY, float upZ);

    static CTransform PerspectiveFovRH(float fovYDeg, float aspect,
        float nearZ, float farZ);

    void TransformPoint(float x, float y, float z,
        float& outX, float& outY, float& outZ, float& outW) const;

    void TransformDirection(float x, float y, float z,
        float& outX, float& outY, float& outZ) const;
};
