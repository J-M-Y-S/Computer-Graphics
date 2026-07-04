
// FINALView.cpp: CFINALView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "FINAL.h"
#endif

#include "FINALDoc.h"
#include "FINALView.h"
#include "Object3D.h"
#include "Transform.h"
#include "Triangle.h"
#include "Lighting.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFINALView

IMPLEMENT_DYNCREATE(CFINALView, CView)

#define PI 3.14159265358979323846f

BEGIN_MESSAGE_MAP(CFINALView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CFINALView 构造/析构

CFINALView::CFINALView() noexcept
{
	m_bMouseDragging = false;
	m_nTimerID = 0;
}

CFINALView::~CFINALView()
{
}

BOOL CFINALView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CFINALView 绘图

void CFINALView::DrawPixel(DWORD* pBits, int w, int h, int x, int y, DWORD color)
{
	if (x >= 0 && x < w && y >= 0 && y < h)
		pBits[y * w + x] = color;
}

void CFINALView::DrawWireframe(const CObject3D* pModel, const CTransform& mvp,
	DWORD* pBits, int w, int h)
{
	const VertexPNT* verts = pModel->GetVertices();
	const CObject3D::Face* faces = pModel->GetFaces();
	int nFaces = pModel->GetFaceCount();

	for (int i = 0; i < nFaces; i++)
	{
		const CObject3D::Face& f = faces[i];
		float sx[3], sy[3];
		bool valid[3];
		float vx[3] = { verts[f.v0].px, verts[f.v1].px, verts[f.v2].px };
		float vy[3] = { verts[f.v0].py, verts[f.v1].py, verts[f.v2].py };
		float vz[3] = { verts[f.v0].pz, verts[f.v1].pz, verts[f.v2].pz };

		for (int k = 0; k < 3; k++)
		{
			float ox, oy, oz, ow;
			mvp.TransformPoint(vx[k], vy[k], vz[k], ox, oy, oz, ow);
			if (ow > 1e-10f)
			{
				sx[k] = (ox / ow + 1.0f) * 0.5f * w;
				sy[k] = (1.0f - oy / ow) * 0.5f * h;
				valid[k] = true;
			}
			else valid[k] = false;
		}
		if (!valid[0] || !valid[1] || !valid[2]) continue;

		bool allOut = true;
		for (int k = 0; k < 3; k++)
			if (sx[k] >= 0 && sx[k] < w && sy[k] >= 0 && sy[k] < h) { allOut = false; break; }
		if (allOut) continue;

		DWORD color = 0x0000FF00;
		for (int e = 0; e < 3; e++)
		{
			int x0 = (int)sx[e], y0 = (int)sy[e];
			int x1 = (int)sx[(e + 1) % 3], y1 = (int)sy[(e + 1) % 3];
			int dx = abs(x1 - x0), sx_step = x0 < x1 ? 1 : -1;
			int dy = -abs(y1 - y0), sy_step = y0 < y1 ? 1 : -1;
			int err = dx + dy;
			while (true)
			{
				DrawPixel(pBits, w, h, x0, y0, color);
				if (x0 == x1 && y0 == y1) break;
				int e2 = 2 * err;
				if (e2 >= dy) { err += dy; x0 += sx_step; }
				if (e2 <= dx) { err += dx; y0 += sy_step; }
			}
		}
	}
}

void CFINALView::DrawLitModel(const CObject3D* pModel,
	const CTransform& world, const CTransform& view,
	const CTransform& mvp, DWORD* pBits, int w, int h, CZBuffer* pZBuf)
{
	const VertexPNT* verts = pModel->GetVertices();
	const CObject3D::Face* faces = pModel->GetFaces();
	int nFaces = pModel->GetFaceCount();

	CMaterial mat; mat.SetDefaultRed();
	CLightSource light; light.SetPointLight(2.0f, 3.0f, 4.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	float vpx = 0, vpy = 0, vpz = 2.5f;

	for (int i = 0; i < nFaces; i++)
	{
		const CObject3D::Face& f = faces[i];
		const VertexPNT& v0 = verts[f.v0], &v1 = verts[f.v1], &v2 = verts[f.v2];

		float cx = (v0.px + v1.px + v2.px) / 3.0f;
		float cy = (v0.py + v1.py + v2.py) / 3.0f;
		float cz = (v0.pz + v1.pz + v2.pz) / 3.0f;
		float wcx, wcy, wcz, wcw;
		world.TransformPoint(cx, cy, cz, wcx, wcy, wcz, wcw);

		float fnx = (v0.nx + v1.nx + v2.nx) / 3.0f;
		float fny = (v0.ny + v1.ny + v2.ny) / 3.0f;
		float fnz = (v0.nz + v1.nz + v2.nz) / 3.0f;
		float wnx, wny, wnz;
		world.TransformDirection(fnx, fny, fnz, wnx, wny, wnz);

		float vx = vpx - wcx, vy = vpy - wcy, vz = vpz - wcz;
		float vl = sqrtf(vx * vx + vy * vy + vz * vz);
		if (vl > 1e-10f) { vx /= vl; vy /= vl; vz /= vl; }
		if (wnx * vx + wny * vy + wnz * vz <= 0) continue;

		float sx[3], sy[3], sz[3];
		bool valid[3];
		float pvx[3] = { v0.px, v1.px, v2.px };
		float pvy[3] = { v0.py, v1.py, v2.py };
		float pvz[3] = { v0.pz, v1.pz, v2.pz };

		for (int k = 0; k < 3; k++)
		{
			float ox, oy, oz, ow;
			mvp.TransformPoint(pvx[k], pvy[k], pvz[k], ox, oy, oz, ow);
			if (ow > 1e-10f)
			{
				sx[k] = (ox / ow + 1.0f) * 0.5f * w;
				sy[k] = (1.0f - oy / ow) * 0.5f * h;
				sz[k] = oz / ow;
				valid[k] = true;
			}
			else valid[k] = false;
		}
		if (!valid[0] || !valid[1] || !valid[2]) continue;

		bool allOut = true;
		for (int k = 0; k < 3; k++)
			if (sx[k] >= -100 && sx[k] < w + 100 && sy[k] >= -100 && sy[k] < h + 100) { allOut = false; break; }
		if (allOut) continue;

		Vertex2D vt[3];
		const VertexPNT* vp[3] = { &v0, &v1, &v2 };
		for (int k = 0; k < 3; k++)
		{
			vt[k].x = sx[k]; vt[k].y = sy[k]; vt[k].z = sz[k];
			float wpx, wpy, wpz, wpw;
			world.TransformPoint(pvx[k], pvy[k], pvz[k], wpx, wpy, wpz, wpw);
			float wnx, wny, wnz;
			world.TransformDirection(vp[k]->nx, vp[k]->ny, vp[k]->nz, wnx, wny, wnz);
			float nl = sqrtf(wnx * wnx + wny * wny + wnz * wnz);
			if (nl > 1e-10f) { wnx /= nl; wny /= nl; wnz /= nl; }
			CLighting::ComputeVertexLight(wpx, wpy, wpz, wnx, wny, wnz,
				vpx, vpy, vpz, &mat, &light, vt[k].r, vt[k].g, vt[k].b);
			vt[k].u = vp[k]->tu; vt[k].v = vp[k]->tv;
		}
		CTriangle::Rasterize(vt[0], vt[1], vt[2], pZBuf, nullptr, pBits, w, h, 2);
	}
}

void CFINALView::DrawTexturedModel(CObject3D* pModel,
	const CTransform& world, const CTransform& view,
	const CTransform& mvp, DWORD* pBits, int w, int h, CZBuffer* pZBuf)
{
	const VertexPNT* verts = pModel->GetVertices();
	const CObject3D::Face* faces = pModel->GetFaces();
	int nFaces = pModel->GetFaceCount();

	CTexture* pTex = pModel->GetBaseColorTexture();
	if (!pTex || !pTex->IsLoaded()) return;

	CMaterial mat; mat.SetDefaultRed();
	CLightSource light; light.SetPointLight(2.0f, 3.0f, 4.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	float vpx = 0, vpy = 0, vpz = 2.5f;

	for (int i = 0; i < nFaces; i++)
	{
		const CObject3D::Face& f = faces[i];
		const VertexPNT& v0 = verts[f.v0], &v1 = verts[f.v1], &v2 = verts[f.v2];

		float cx = (v0.px + v1.px + v2.px) / 3.0f;
		float cy = (v0.py + v1.py + v2.py) / 3.0f;
		float cz = (v0.pz + v1.pz + v2.pz) / 3.0f;
		float wcx, wcy, wcz, wcw;
		world.TransformPoint(cx, cy, cz, wcx, wcy, wcz, wcw);

		float fnx = (v0.nx + v1.nx + v2.nx) / 3.0f;
		float fny = (v0.ny + v1.ny + v2.ny) / 3.0f;
		float fnz = (v0.nz + v1.nz + v2.nz) / 3.0f;
		float wnx, wny, wnz;
		world.TransformDirection(fnx, fny, fnz, wnx, wny, wnz);

		float vx = vpx - wcx, vy = vpy - wcy, vz = vpz - wcz;
		float vl = sqrtf(vx * vx + vy * vy + vz * vz);
		if (vl > 1e-10f) { vx /= vl; vy /= vl; vz /= vl; }
		if (wnx * vx + wny * vy + wnz * vz <= 0) continue;

		float sx[3], sy[3], sz[3];
		bool valid[3];
		float pvx[3] = { v0.px, v1.px, v2.px };
		float pvy[3] = { v0.py, v1.py, v2.py };
		float pvz[3] = { v0.pz, v1.pz, v2.pz };

		for (int k = 0; k < 3; k++)
		{
			float ox, oy, oz, ow;
			mvp.TransformPoint(pvx[k], pvy[k], pvz[k], ox, oy, oz, ow);
			if (ow > 1e-10f)
			{
				sx[k] = (ox / ow + 1.0f) * 0.5f * w;
				sy[k] = (1.0f - oy / ow) * 0.5f * h;
				sz[k] = oz / ow;
				valid[k] = true;
			}
			else valid[k] = false;
		}
		if (!valid[0] || !valid[1] || !valid[2]) continue;

		bool allOut = true;
		for (int k = 0; k < 3; k++)
			if (sx[k] >= -100 && sx[k] < w + 100 && sy[k] >= -100 && sy[k] < h + 100) { allOut = false; break; }
		if (allOut) continue;

		Vertex2D vt[3];
		const VertexPNT* vp[3] = { &v0, &v1, &v2 };
		for (int k = 0; k < 3; k++)
		{
			vt[k].x = sx[k]; vt[k].y = sy[k]; vt[k].z = sz[k];
			float wpx, wpy, wpz, wpw;
			world.TransformPoint(pvx[k], pvy[k], pvz[k], wpx, wpy, wpz, wpw);
			float wnx, wny, wnz;
			world.TransformDirection(vp[k]->nx, vp[k]->ny, vp[k]->nz, wnx, wny, wnz);
			float nl = sqrtf(wnx * wnx + wny * wny + wnz * wnz);
			if (nl > 1e-10f) { wnx /= nl; wny /= nl; wnz /= nl; }
			CLighting::ComputeVertexLight(wpx, wpy, wpz, wnx, wny, wnz,
				vpx, vpy, vpz, &mat, &light, vt[k].r, vt[k].g, vt[k].b);
			vt[k].u = vp[k]->tu; vt[k].v = vp[k]->tv;
		}
		CTriangle::Rasterize(vt[0], vt[1], vt[2], pZBuf, pTex, pBits, w, h, 3);
	}
}

void CFINALView::RenderFrame(CDC* pDC)
{
	CFINALDoc* pDoc = GetDocument();
	CObject3D* pModel = pDoc->GetModel();
	if (!pModel->IsLoaded()) return;

	CRect rect; GetClientRect(&rect);
	int w = rect.Width(), h = rect.Height();
	if (w <= 0 || h <= 0) return;

	if (pDoc->m_bAutoRotate && m_nTimerID == 0)
		m_nTimerID = SetTimer(1, 33, NULL);
	else if (!pDoc->m_bAutoRotate && m_nTimerID != 0)
		{ KillTimer(m_nTimerID); m_nTimerID = 0; }

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
	bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	DWORD* pBits = nullptr;
	HBITMAP hBmp = CreateDIBSection(pDC->GetSafeHdc(), &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	HDC hMemDC = CreateCompatibleDC(pDC->GetSafeHdc());
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	memset(pBits, 0x30, w * h * 4);

	CZBuffer zBuf; zBuf.Create(w, h); zBuf.Clear();

	float aspect = (float)w / (float)h;
	float scale = pModel->GetNormalizeScale();
	float camDist = 2.5f * pDoc->m_zoom;

	CTransform world = CTransform::RotationY(pDoc->m_rotY);
	world.Multiply(CTransform::RotationX(pDoc->m_rotX));
	world.Multiply(CTransform::Scaling(scale, scale, scale));
	world.Multiply(CTransform::Translation(-pModel->GetCenterX(), -pModel->GetCenterY(), -pModel->GetCenterZ()));
	CTransform view = CTransform::LookAt(0, 0, camDist, 0, 0, 0, 0, 1, 0);
	CTransform proj = CTransform::PerspectiveFovRH(60.0f, aspect, 0.1f, 100.0f);
	CTransform mvp = world; mvp.Multiply(view); mvp.Multiply(proj);

	if (pDoc->m_bWireframe) DrawWireframe(pModel, mvp, pBits, w, h);
	if (pDoc->m_bLit) DrawLitModel(pModel, world, view, mvp, pBits, w, h, &zBuf);
	if (pDoc->m_bTextured) DrawTexturedModel(pModel, world, view, mvp, pBits, w, h, &zBuf);

	BitBlt(pDC->GetSafeHdc(), 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
	DeleteObject(hBmp);
}

void CFINALView::OnDraw(CDC* pDC)
{
	CFINALDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	RenderFrame(pDC);
}


// CFINALView 打印

BOOL CFINALView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CFINALView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CFINALView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CFINALView 诊断

#ifdef _DEBUG
void CFINALView::AssertValid() const
{
	CView::AssertValid();
}

void CFINALView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFINALDoc* CFINALView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFINALDoc)));
	return (CFINALDoc*)m_pDocument;
}
#endif //_DEBUG


// CFINALView 消息处理程序

void CFINALView::OnTimer(UINT_PTR nIDEvent)
{
	CFINALDoc* pDoc = GetDocument();
	if (pDoc && pDoc->m_bAutoRotate)
	{
		pDoc->m_rotY += 1.0f;
		if (pDoc->m_rotY >= 360.0f) pDoc->m_rotY -= 360.0f;
		Invalidate(FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void CFINALView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (cx > 0 && cy > 0) m_projection.Setup(cx, cy, 60.0f, 0.1f, 100.0f);
}

void CFINALView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_lastMousePos = point; m_bMouseDragging = true; SetCapture();
	CView::OnLButtonDown(nFlags, point);
}

void CFINALView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMouseDragging && (nFlags & MK_LBUTTON))
	{
		CFINALDoc* pDoc = GetDocument();
		float dx = (float)(point.x - m_lastMousePos.x);
		float dy = (float)(point.y - m_lastMousePos.y);
		pDoc->m_rotY += dx * 0.5f; pDoc->m_rotX += dy * 0.5f;
		m_lastMousePos = point; Invalidate(FALSE);
	}
	CView::OnMouseMove(nFlags, point);
}

void CFINALView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDragging = false; ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

BOOL CFINALView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CFINALDoc* pDoc = GetDocument();
	pDoc->m_zoom -= zDelta * 0.001f;
	if (pDoc->m_zoom < 0.1f) pDoc->m_zoom = 0.1f;
	if (pDoc->m_zoom > 10.0f) pDoc->m_zoom = 10.0f;
	Invalidate(FALSE);
	return TRUE;
}
