
// FINALView.h: CFINALView 类的接口
//

#pragma once

#include "Projection.h"
#include "ZBuffer.h"
#include "Lighting.h"
#include "Texture.h"


class CFINALView : public CView
{
protected: // 仅从序列化创建
	CFINALView() noexcept;
	DECLARE_DYNCREATE(CFINALView)

// 特性
public:
	CFINALDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CFINALView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CProjection m_projection;
	CPoint m_lastMousePos;
	bool m_bMouseDragging;
	UINT_PTR m_nTimerID;

	void RenderFrame(CDC* pDC);
	void DrawWireframe(const CObject3D* pModel, const CTransform& mvp,
		DWORD* pBits, int w, int h);
	void DrawLitModel(const CObject3D* pModel,
		const CTransform& world, const CTransform& view,
		const CTransform& mvp,
		DWORD* pBits, int w, int h, CZBuffer* pZBuf);
	void DrawTexturedModel(CObject3D* pModel,
		const CTransform& world, const CTransform& view,
		const CTransform& mvp,
		DWORD* pBits, int w, int h, CZBuffer* pZBuf);
	void DrawPixel(DWORD* pBits, int w, int h, int x, int y, DWORD color);

// 生成的消息映射函数
protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // FINALView.cpp 中的调试版本
inline CFINALDoc* CFINALView::GetDocument() const
   { return reinterpret_cast<CFINALDoc*>(m_pDocument); }
#endif

