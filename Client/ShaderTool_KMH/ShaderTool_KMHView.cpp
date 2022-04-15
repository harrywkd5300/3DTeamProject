
// ShaderTool_KMHView.cpp : CShaderTool_KMHView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "ShaderTool_KMH.h"
#endif

#include "ShaderTool_KMHDoc.h"
#include "ShaderTool_KMHView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShaderTool_KMHView

IMPLEMENT_DYNCREATE(CShaderTool_KMHView, CView)

BEGIN_MESSAGE_MAP(CShaderTool_KMHView, CView)
END_MESSAGE_MAP()

// CShaderTool_KMHView ����/�Ҹ�

CShaderTool_KMHView::CShaderTool_KMHView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CShaderTool_KMHView::~CShaderTool_KMHView()
{
}

BOOL CShaderTool_KMHView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CShaderTool_KMHView �׸���

void CShaderTool_KMHView::OnDraw(CDC* /*pDC*/)
{
	CShaderTool_KMHDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CShaderTool_KMHView ����

#ifdef _DEBUG
void CShaderTool_KMHView::AssertValid() const
{
	CView::AssertValid();
}

void CShaderTool_KMHView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShaderTool_KMHDoc* CShaderTool_KMHView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShaderTool_KMHDoc)));
	return (CShaderTool_KMHDoc*)m_pDocument;
}
#endif //_DEBUG


// CShaderTool_KMHView �޽��� ó����
