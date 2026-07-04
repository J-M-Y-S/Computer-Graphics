#include "pch.h"
#include "Object3D.h"
#include "Transform.h"
#include "Lighting.h"
#include "Texture.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

CObject3D::CObject3D()
{
    m_bLoaded = false;
    m_pMaterial = nullptr;
    m_pTexBaseColor = nullptr;
    m_pTexMetallic = nullptr;
    m_pTexRoughness = nullptr;
    m_pTexNormalMap = nullptr;
    m_bboxMin[0] = m_bboxMin[1] = m_bboxMin[2] = 0;
    m_bboxMax[0] = m_bboxMax[1] = m_bboxMax[2] = 0;
}

CObject3D::~CObject3D()
{
    delete m_pMaterial;
    delete m_pTexBaseColor;
    delete m_pTexMetallic;
    delete m_pTexRoughness;
    delete m_pTexNormalMap;
}

bool CObject3D::ParseOBJ(const CString& objPath)
{
    FILE* fp = _wfopen(objPath.GetString(), L"rb");
    if (!fp)
    {
        fp = fopen(CT2A(objPath), "rb");
        if (!fp) return false;
    }

    std::vector<float> tempPos;
    std::vector<float> tempTex;
    std::vector<Face> tempFaces;
    std::unordered_map<std::string, int> vertexMap;

    tempPos.reserve(750002 * 3);
    tempTex.reserve(800000 * 2);
    tempFaces.reserve(1500000);

    const int BUF_SIZE = 512;
    char line[BUF_SIZE];

    while (fgets(line, BUF_SIZE, fp))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            float x, y, z;
            if (sscanf(line + 2, "%f %f %f", &x, &y, &z) == 3)
            {
                tempPos.push_back(x);
                tempPos.push_back(y);
                tempPos.push_back(z);
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            float u, v;
            if (sscanf(line + 3, "%f %f", &u, &v) == 2)
            {
                tempTex.push_back(u);
                tempTex.push_back(v);
            }
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int vi[3], ti[3];
            int n = sscanf(line + 2, "%d/%d %d/%d %d/%d",
                &vi[0], &ti[0], &vi[1], &ti[1], &vi[2], &ti[2]);
            if (n >= 6)
            {
                Face face;
                for (int k = 0; k < 3; k++)
                {
                    char key[64];
                    sprintf(key, "%d/%d", vi[k], ti[k]);
                    auto it = vertexMap.find(key);
                    if (it != vertexMap.end())
                    {
                        (k == 0 ? face.v0 : (k == 1 ? face.v1 : face.v2)) = it->second;
                    }
                    else
                    {
                        int idx = (int)m_vertices.size();
                        VertexPNT vtx = {};
                        int pi = (vi[k] - 1) * 3;
                        if (pi >= 0 && pi + 2 < (int)tempPos.size())
                        {
                            vtx.px = tempPos[pi];
                            vtx.py = tempPos[pi + 1];
                            vtx.pz = tempPos[pi + 2];
                        }
                        int tpi = (ti[k] - 1) * 2;
                        if (tpi >= 0 && tpi + 1 < (int)tempTex.size())
                        {
                            vtx.tu = tempTex[tpi];
                            vtx.tv = tempTex[tpi + 1];
                        }
                        m_vertices.push_back(vtx);
                        vertexMap[key] = idx;
                        (k == 0 ? face.v0 : (k == 1 ? face.v1 : face.v2)) = idx;
                    }
                }
                tempFaces.push_back(face);
            }
        }
    }
    fclose(fp);

    m_faces = std::move(tempFaces);

    ComputeVertexNormals();
    ComputeBoundingBox();

    return m_vertices.size() > 0 && m_faces.size() > 0;
}

void CObject3D::ComputeVertexNormals()
{
    for (auto& v : m_vertices)
        v.nx = v.ny = v.nz = 0;

    const Face* faces = m_faces.data();
    VertexPNT* verts = m_vertices.data();
    int nFaces = (int)m_faces.size();

    for (int i = 0; i < nFaces; i++)
    {
        const Face& f = faces[i];
        VertexPNT& v0 = verts[f.v0];
        VertexPNT& v1 = verts[f.v1];
        VertexPNT& v2 = verts[f.v2];

        float e1x = v1.px - v0.px, e1y = v1.py - v0.py, e1z = v1.pz - v0.pz;
        float e2x = v2.px - v0.px, e2y = v2.py - v0.py, e2z = v2.pz - v0.pz;

        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;
        float len = sqrtf(nx * nx + ny * ny + nz * nz);
        if (len > 1e-10f) { nx /= len; ny /= len; nz /= len; }

        v0.nx += nx; v0.ny += ny; v0.nz += nz;
        v1.nx += nx; v1.ny += ny; v1.nz += nz;
        v2.nx += nx; v2.ny += ny; v2.nz += nz;
    }

    for (auto& v : m_vertices)
    {
        float len = sqrtf(v.nx * v.nx + v.ny * v.ny + v.nz * v.nz);
        if (len > 1e-10f) { v.nx /= len; v.ny /= len; v.nz /= len; }
    }
}

void CObject3D::ComputeBoundingBox()
{
    if (m_vertices.empty()) return;
    m_bboxMin[0] = m_bboxMin[1] = m_bboxMin[2] = 1e10f;
    m_bboxMax[0] = m_bboxMax[1] = m_bboxMax[2] = -1e10f;

    for (const auto& v : m_vertices)
    {
        if (v.px < m_bboxMin[0]) m_bboxMin[0] = v.px;
        if (v.py < m_bboxMin[1]) m_bboxMin[1] = v.py;
        if (v.pz < m_bboxMin[2]) m_bboxMin[2] = v.pz;
        if (v.px > m_bboxMax[0]) m_bboxMax[0] = v.px;
        if (v.py > m_bboxMax[1]) m_bboxMax[1] = v.py;
        if (v.pz > m_bboxMax[2]) m_bboxMax[2] = v.pz;
    }
}

float CObject3D::GetNormalizeScale() const
{
    float dx = m_bboxMax[0] - m_bboxMin[0];
    float dy = m_bboxMax[1] - m_bboxMin[1];
    float dz = m_bboxMax[2] - m_bboxMin[2];
    float maxExtent = dx;
    if (dy > maxExtent) maxExtent = dy;
    if (dz > maxExtent) maxExtent = dz;
    if (maxExtent < 1e-6f) return 1.0f;
    return 2.0f / maxExtent;
}

bool CObject3D::ParseMTL(const CString& mtlPath, const CString& texturesDir)
{
	FILE* fp = _wfopen(mtlPath.GetString(), L"rb");
	if (!fp)
	{
		fp = fopen(CT2A(mtlPath), "rb");
		if (!fp) return false;
	}

	char line[512];
	CString baseColorPath;

	while (fgets(line, 512, fp))
	{
		if (strncmp(line, "Kd ", 3) == 0)
		{
			float r, g, b;
			if (sscanf(line + 3, "%f %f %f", &r, &g, &b) == 3)
			{
				if (!m_pMaterial) m_pMaterial = new CMaterial();
				m_pMaterial->SetFromMTL(r, g, b);
			}
		}
		else if (strncmp(line, "map_Kd ", 7) == 0)
		{
			char texName[256];
			if (sscanf(line + 7, "%255s", texName) == 1)
				baseColorPath = CString(texName);
		}
	}
	fclose(fp);

	if (!baseColorPath.IsEmpty())
	{
		CString fullPath = texturesDir + _T("\\") + baseColorPath;
		if (!m_pTexBaseColor) m_pTexBaseColor = new CTexture();
		m_pTexBaseColor->Load(fullPath);
	}

	return true;
}

bool CObject3D::Load(const CString& objPath, const CString& texturesDir)
{
	m_bLoaded = ParseOBJ(objPath);
	if (m_bLoaded)
	{
		CString mtlPath = texturesDir + _T("\\material.mtl");
		ParseMTL(mtlPath, texturesDir);
	}
	return m_bLoaded;
}
