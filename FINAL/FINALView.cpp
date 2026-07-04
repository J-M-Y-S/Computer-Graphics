
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFINALView

IMPLEMENT_DYNCREATE(CFINALView, CView)

BEGIN_MESSAGE_MAP(CFINALView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CFINALView 构造/析构

CFINALView::CFINALView() noexcept
{
	// TODO: 在此处添加构造代码

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

void CFINALView::OnDraw(CDC* /*pDC*/)
{
	CFINALDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
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
