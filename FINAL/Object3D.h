#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct VertexPNT
{
    float px, py, pz;
    float nx, ny, nz;
    float tu, tv;
};

class CTexture;
class CMaterial;

class CObject3D
{
public:
    struct Face { int v0, v1, v2; };

    CObject3D();
    ~CObject3D();

    bool Load(const CString& objPath, const CString& texturesDir);
    bool IsLoaded() const { return m_bLoaded; }

    int GetVertexCount() const { return (int)m_vertices.size(); }
    int GetFaceCount() const { return (int)m_faces.size(); }
    const VertexPNT* GetVertices() const { return m_vertices.data(); }
    const Face* GetFaces() const { return m_faces.data(); }

    float GetMinX() const { return m_bboxMin[0]; }
    float GetMinY() const { return m_bboxMin[1]; }
    float GetMinZ() const { return m_bboxMin[2]; }
    float GetMaxX() const { return m_bboxMax[0]; }
    float GetMaxY() const { return m_bboxMax[1]; }
    float GetMaxZ() const { return m_bboxMax[2]; }
    float GetCenterX() const { return (m_bboxMin[0] + m_bboxMax[0]) * 0.5f; }
    float GetCenterY() const { return (m_bboxMin[1] + m_bboxMax[1]) * 0.5f; }
    float GetCenterZ() const { return (m_bboxMin[2] + m_bboxMax[2]) * 0.5f; }
    float GetNormalizeScale() const;

    CMaterial* GetMaterial() { return m_pMaterial; }
    CTexture* GetBaseColorTexture() { return m_pTexBaseColor; }
    CTexture* GetMetallicTexture() { return m_pTexMetallic; }
    CTexture* GetRoughnessTexture() { return m_pTexRoughness; }
    CTexture* GetNormalMapTexture() { return m_pTexNormalMap; }

private:
    std::vector<VertexPNT> m_vertices;
    std::vector<Face> m_faces;
    CMaterial* m_pMaterial;
    CTexture* m_pTexBaseColor;
    CTexture* m_pTexMetallic;
    CTexture* m_pTexRoughness;
    CTexture* m_pTexNormalMap;
    bool m_bLoaded;
    float m_bboxMin[3], m_bboxMax[3];

    bool ParseOBJ(const CString& objPath);
    bool ParseMTL(const CString& mtlPath, const CString& texturesDir);
    void ComputeVertexNormals();
    void ComputeBoundingBox();
};
