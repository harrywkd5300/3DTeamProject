// ../Codes/EditBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "EditBox.h"

#include "EditSheet.h"


// CEditBox

IMPLEMENT_DYNCREATE(CEditBox, CFormView)

CEditBox::CEditBox()
	: CFormView(IDD_EDITBOX)
{

}

CEditBox::~CEditBox()
{
	Engine::Safe_Delete( m_pEditSheet );
}

void CEditBox::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditBox, CFormView)
END_MESSAGE_MAP()


// CEditBox �����Դϴ�.

#ifdef _DEBUG
void CEditBox::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEditBox::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEditBox �޽��� ó�����Դϴ�.


void CEditBox::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_pEditSheet = new CEditSheet;

	m_pEditSheet->Create( this, WS_CHILD | WS_VISIBLE );
	
	m_pEditSheet->MoveWindow( 0, 0, TOOLCX, WINCY );
}
