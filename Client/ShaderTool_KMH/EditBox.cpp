// ../Codes/EditBox.cpp : 구현 파일입니다.
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


// CEditBox 진단입니다.

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


// CEditBox 메시지 처리기입니다.


void CEditBox::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pEditSheet = new CEditSheet;

	m_pEditSheet->Create( this, WS_CHILD | WS_VISIBLE );
	
	m_pEditSheet->MoveWindow( 0, 0, TOOLCX, WINCY );
}
