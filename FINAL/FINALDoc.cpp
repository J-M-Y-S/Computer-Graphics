
// FINALDoc.cpp: CFINALDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "FINAL.h"
#endif

#include "FINALDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFINALDoc

IMPLEMENT_DYNCREATE(CFINALDoc, CDocument)

BEGIN_MESSAGE_MAP(CFINALDoc, CDocument)
END_MESSAGE_MAP()


// CFINALDoc 构造/析构

CFINALDoc::CFINALDoc() noexcept
{
	m_bWireframe = true;
	m_bLit = false;
	m_bTextured = false;
	m_bAutoRotate = false;
	m_rotY = 0;
	m_rotX = 0;
	m_zoom = 1.0f;
}

CFINALDoc::~CFINALDoc()
{
}

BOOL CFINALDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString exeDir;
	GetModuleFileName(AfxGetInstanceHandle(), exeDir.GetBuffer(MAX_PATH), MAX_PATH);
	exeDir.ReleaseBuffer();
	int pos = exeDir.ReverseFind(_T('\\'));
	if (pos >= 0) exeDir = exeDir.Left(pos);

	CString objPath = exeDir + _T("\\textures\\bae891247d7923349b1b693f25426e56.obj");
	CString texDir = exeDir + _T("\\textures");
	if (!m_model.Load(objPath, texDir))
	{
		objPath = _T("E:\\PY\\FINAL\\FINAL\\textures\\bae891247d7923349b1b693f25426e56.obj");
		texDir = _T("E:\\PY\\FINAL\\FINAL\\textures");
		m_model.Load(objPath, texDir);
	}

	return TRUE;
}




// CFINALDoc 序列化

void CFINALDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CFINALDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CFINALDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CFINALDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFINALDoc 诊断

#ifdef _DEBUG
void CFINALDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFINALDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CFINALDoc::LoadModel(const CString& objPath, const CString& texturesDir)
{
	return m_model.Load(objPath, texturesDir) ? TRUE : FALSE;
}

// CFINALDoc 命令
