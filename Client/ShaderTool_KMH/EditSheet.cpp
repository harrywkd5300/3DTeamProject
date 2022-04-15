// ../Codes/EditSheet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "EditSheet.h"

#include "Tool_Shader.h"
#include "Tool_Light.h"
#include "Tool_Interaction.h"


// CEditSheet

IMPLEMENT_DYNAMIC(CEditSheet, CPropertySheet)

CEditSheet::CEditSheet( void )
	: m_pTool_Shader( new CTool_Shader )
	, m_pTool_Light( new CTool_Light )
	, m_pTool_Interaction( new CTool_Interaction )
{
	// Create And Add Shader Tool..
	AddPage( m_pTool_Shader );
	m_vecTool.push_back( m_pTool_Shader );

	AddPage( m_pTool_Light );
	m_vecTool.push_back( m_pTool_Light );

	AddPage( m_pTool_Interaction );
	m_vecTool.push_back( m_pTool_Interaction );
}

CEditSheet::CEditSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CEditSheet::CEditSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CEditSheet::~CEditSheet()
{
	for ( auto& Tool : m_vecTool )
		Engine::Safe_Delete( Tool );
}


BEGIN_MESSAGE_MAP(CEditSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEditSheet 메시지 처리기입니다.
