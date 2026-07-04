#pragma once

class CZBuffer;
class CTexture;

struct Vertex2D
{
	float x, y, z;
	float r, g, b;
	float u, v;
};

class CTriangle
{
public:
	// mode: 0=wireframe edges, 1=flat fill, 2=Gouraud fill, 3=textured
	static void Rasterize(const Vertex2D& v0, const Vertex2D& v1, const Vertex2D& v2,
		CZBuffer* pZBuf, CTexture* pTex,
		DWORD* pBits, int width, int height, int mode);

private:
	static void FillSpan(int y, float xLeft, float xRight,
		float zLeft, float zRight,
		float rLeft, float rRight, float gLeft, float gRight,
		float bLeft, float bRight,
		float uLeft, float uRight, float vLeft, float vRight,
		CZBuffer* pZBuf, CTexture* pTex,
		DWORD* pBits, int width, int height, int mode);
};
