#include "pch.h"
#include "Triangle.h"
#include "ZBuffer.h"
#include "Texture.h"
#include <cmath>
#include <algorithm>

static inline int Clamp(int v, int lo, int hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

static inline float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

static inline DWORD FloatColorToDWORD(float r, float g, float b)
{
	int ir = Clamp((int)(r * 255), 0, 255);
	int ig = Clamp((int)(g * 255), 0, 255);
	int ib = Clamp((int)(b * 255), 0, 255);
	return 0xFF000000 | (ib << 16) | (ig << 8) | ir;
}

void CTriangle::FillSpan(int y, float xLeft, float xRight,
	float zLeft, float zRight,
	float rLeft, float rRight, float gLeft, float gRight,
	float bLeft, float bRight,
	float uLeft, float uRight, float vLeft, float vRight,
	CZBuffer* pZBuf, CTexture* pTex,
	DWORD* pBits, int width, int height, int mode)
{
	if (y < 0 || y >= height) return;

	int x0 = Clamp((int)ceilf(xLeft), 0, width - 1);
	int x1 = Clamp((int)floorf(xRight), 0, width - 1);
	if (x0 > x1) return;

	float dx = xRight - xLeft;
	float invDx = (fabsf(dx) > 1e-10f) ? 1.0f / dx : 0.0f;

	for (int x = x0; x <= x1; x++)
	{
		float t = (x - xLeft) * invDx;
		float z = Lerp(zLeft, zRight, t);

		DWORD color = 0;
		if (mode == 1 || mode == 2)
		{
			float r = Lerp(rLeft, rRight, t);
			float g = Lerp(gLeft, gRight, t);
			float b = Lerp(bLeft, bRight, t);
			color = FloatColorToDWORD(r, g, b);
		}
		else if (mode == 3 && pTex)
		{
			float u = Lerp(uLeft, uRight, t);
			float v = Lerp(vLeft, vRight, t);
			color = pTex->SampleBilinear(u, v);
		}
		else
		{
			color = FloatColorToDWORD(0.8f, 0.2f, 0.2f); // default red
		}

		if (pZBuf->TestAndSet(x, y, z))
		{
			pBits[y * width + x] = color;
		}
	}
}

void CTriangle::Rasterize(const Vertex2D& v0, const Vertex2D& v1, const Vertex2D& v2,
	CZBuffer* pZBuf, CTexture* pTex,
	DWORD* pBits, int width, int height, int mode)
{
	if (!pZBuf || !pBits) return;

	// Sort by Y (ascending): v[0]=top, v[2]=bottom
	const Vertex2D* v[3] = { &v0, &v1, &v2 };
	if (v[0]->y > v[1]->y) std::swap(v[0], v[1]);
	if (v[1]->y > v[2]->y) std::swap(v[1], v[2]);
	if (v[0]->y > v[1]->y) std::swap(v[0], v[1]);

	float y0 = v[0]->y, y1 = v[1]->y, y2 = v[2]->y;

	// Degenerate triangle
	if (y2 - y0 < 1e-6f) return;

	int iY0 = Clamp((int)ceilf(y0), 0, height - 1);
	int iY1 = Clamp((int)ceilf(y1), 0, height - 1);
	int iY2 = Clamp((int)floorf(y2), 0, height - 1);

	// Precompute edge slopes for left and right edges
	float invDy02 = (y2 - y0 > 1e-10f) ? 1.0f / (y2 - y0) : 0.0f;
	float invDy01 = (y1 - y0 > 1e-10f) ? 1.0f / (y1 - y0) : 0.0f;
	float invDy12 = (y2 - y1 > 1e-10f) ? 1.0f / (y2 - y1) : 0.0f;

	// Determine which side is left vs right
	bool midIsRight = (v[1]->x - v[0]->x) * (y2 - y0) > (v[2]->x - v[0]->x) * (y1 - y0);

	auto InterpEdge = [](float y, float yStart, float xStart, float zStart,
		float r, float g, float b, float u, float v,
		float invDy, float& ox, float& oz, float& oR, float& oG, float& oB,
		float& oU, float& oV)
		{
			float t = (y - yStart) * invDy;
			ox = xStart;
			oz = zStart;
			oR = r;
			oG = g;
			oB = b;
			oU = u;
			oV = v;
			return t;
		};

	// Upper sub-triangle (y0 to y1)
	for (int y = iY0; y <= iY1 && y <= iY2; y++)
	{
		float fy = (float)y;
		float t02 = (fy - y0) * invDy02;
		float t01 = (fy - y0) * invDy01;

		float x02 = Lerp(v[0]->x, v[2]->x, t02);
		float z02 = Lerp(v[0]->z, v[2]->z, t02);
		float r02 = Lerp(v[0]->r, v[2]->r, t02);
		float g02 = Lerp(v[0]->g, v[2]->g, t02);
		float b02 = Lerp(v[0]->b, v[2]->b, t02);
		float u02 = Lerp(v[0]->u, v[2]->u, t02);
		float vv02 = Lerp(v[0]->v, v[2]->v, t02);

		float x01 = Lerp(v[0]->x, v[1]->x, t01);
		float z01 = Lerp(v[0]->z, v[1]->z, t01);
		float r01 = Lerp(v[0]->r, v[1]->r, t01);
		float g01 = Lerp(v[0]->g, v[1]->g, t01);
		float b01 = Lerp(v[0]->b, v[1]->b, t01);
		float u01 = Lerp(v[0]->u, v[1]->u, t01);
		float vv01 = Lerp(v[0]->v, v[1]->v, t01);

		float xL, xR, zL, zR, rL, rR, gL, gR, bL, bR, uL, uR, vL, vR;
		if (midIsRight)
		{
			xL = x02; zL = z02; rL = r02; gL = g02; bL = b02; uL = u02; vL = vv02;
			xR = x01; zR = z01; rR = r01; gR = g01; bR = b01; uR = u01; vR = vv01;
		}
		else
		{
			xL = x01; zL = z01; rL = r01; gL = g01; bL = b01; uL = u01; vL = vv01;
			xR = x02; zR = z02; rR = r02; gR = g02; bR = b02; uR = u02; vR = vv02;
		}
		if (xL > xR) { std::swap(xL, xR); std::swap(zL, zR); std::swap(rL, rR); std::swap(gL, gR); std::swap(bL, bR); std::swap(uL, uR); std::swap(vL, vR); }

		FillSpan(y, xL, xR, zL, zR, rL, rR, gL, gR, bL, bR, uL, uR, vL, vR,
			pZBuf, pTex, pBits, width, height, mode);
	}

	// Lower sub-triangle (y1 to y2)
	for (int y = iY1 + 1; y <= iY2; y++)
	{
		float fy = (float)y;
		float t02 = (fy - y0) * invDy02;
		float t12 = (fy - y1) * invDy12;

		float x02 = Lerp(v[0]->x, v[2]->x, t02);
		float z02 = Lerp(v[0]->z, v[2]->z, t02);
		float r02 = Lerp(v[0]->r, v[2]->r, t02);
		float g02 = Lerp(v[0]->g, v[2]->g, t02);
		float b02 = Lerp(v[0]->b, v[2]->b, t02);
		float u02 = Lerp(v[0]->u, v[2]->u, t02);
		float vv02 = Lerp(v[0]->v, v[2]->v, t02);

		float x12 = Lerp(v[1]->x, v[2]->x, t12);
		float z12 = Lerp(v[1]->z, v[2]->z, t12);
		float r12 = Lerp(v[1]->r, v[2]->r, t12);
		float g12 = Lerp(v[1]->g, v[2]->g, t12);
		float b12 = Lerp(v[1]->b, v[2]->b, t12);
		float u12 = Lerp(v[1]->u, v[2]->u, t12);
		float vv12 = Lerp(v[1]->v, v[2]->v, t12);

		float xL, xR, zL, zR, rL, rR, gL, gR, bL, bR, uL, uR, vL, vR;
		if (midIsRight)
		{
			xL = x02; zL = z02; rL = r02; gL = g02; bL = b02; uL = u02; vL = vv02;
			xR = x12; zR = z12; rR = r12; gR = g12; bR = b12; uR = u12; vR = vv12;
		}
		else
		{
			xL = x12; zL = z12; rL = r12; gL = g12; bL = b12; uL = u12; vL = vv12;
			xR = x02; zR = z02; rR = r02; gR = g02; bR = b02; uR = u02; vR = vv02;
		}
		if (xL > xR) { std::swap(xL, xR); std::swap(zL, zR); std::swap(rL, rR); std::swap(gL, gR); std::swap(bL, bR); std::swap(uL, uR); std::swap(vL, vR); }

		FillSpan(y, xL, xR, zL, zR, rL, rR, gL, gR, bL, bR, uL, uR, vL, vR,
			pZBuf, pTex, pBits, width, height, mode);
	}
}
