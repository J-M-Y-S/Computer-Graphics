
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "FINAL.h"

#include "MainFrm.h"
#include "FINALDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_WIREFRAME,   &CMainFrame::OnViewWireframe)
	ON_COMMAND(ID_VIEW_LIT,         &CMainFrame::OnViewLit)
	ON_COMMAND(ID_VIEW_TEXTURED,    &CMainFrame::OnViewTextured)
	ON_COMMAND(ID_VIEW_AUTOROTATE,  &CMainFrame::OnViewAutoRotate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME,  &CMainFrame::OnUpdateViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIT,        &CMainFrame::OnUpdateViewLit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXTURED,   &CMainFrame::OnUpdateViewTextured)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTOROTATE, &CMainFrame::OnUpdateViewAutoRotate)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		CMenu subMenu;
		subMenu.CreatePopupMenu();
		subMenu.AppendMenu(MF_STRING, ID_VIEW_WIREFRAME,  _T("线框模型(&W)\tCtrl+1"));
		subMenu.AppendMenu(MF_STRING, ID_VIEW_LIT,        _T("光照模型(&L)\tCtrl+2"));
		subMenu.AppendMenu(MF_STRING, ID_VIEW_TEXTURED,   _T("纹理模型(&T)\tCtrl+3"));
		subMenu.AppendMenu(MF_SEPARATOR);
		subMenu.AppendMenu(MF_STRING, ID_VIEW_AUTOROTATE, _T("自转动画(&A)\tCtrl+4"));
		pMenu->InsertMenu(3, MF_BYPOSITION | MF_POPUP, (UINT_PTR)subMenu.Detach(), _T("3D视图(&D)"));
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewWireframe()
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) { pDoc->m_bWireframe = !pDoc->m_bWireframe; GetActiveView()->Invalidate(); }
}
void CMainFrame::OnViewLit()
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) { pDoc->m_bLit = !pDoc->m_bLit; GetActiveView()->Invalidate(); }
}
void CMainFrame::OnViewTextured()
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) { pDoc->m_bTextured = !pDoc->m_bTextured; GetActiveView()->Invalidate(); }
}
void CMainFrame::OnViewAutoRotate()
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) { pDoc->m_bAutoRotate = !pDoc->m_bAutoRotate; GetActiveView()->Invalidate(); }
}
void CMainFrame::OnUpdateViewWireframe(CCmdUI* pCmdUI)
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) pCmdUI->SetCheck(pDoc->m_bWireframe);
}
void CMainFrame::OnUpdateViewLit(CCmdUI* pCmdUI)
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) pCmdUI->SetCheck(pDoc->m_bLit);
}
void CMainFrame::OnUpdateViewTextured(CCmdUI* pCmdUI)
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) pCmdUI->SetCheck(pDoc->m_bTextured);
}
void CMainFrame::OnUpdateViewAutoRotate(CCmdUI* pCmdUI)
{
	CFINALDoc* pDoc = (CFINALDoc*)GetActiveDocument();
	if (pDoc) pCmdUI->SetCheck(pDoc->m_bAutoRotate);
}

