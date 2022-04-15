#pragma once
#include "Tool_Defines.h"
#include "Engine_Defines.h"


// CEditSheet
class CPropertyPage;
class CTool_Shader;
class CTool_Light;
class CTool_Interaction;
class CEditSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEditSheet)

public:
	explicit CEditSheet( void );
	CEditSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CEditSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CEditSheet();
public:
	CTool_Shader* Get_ShaderTool( void ) { return m_pTool_Shader; }
	CTool_Light* Get_LightTool( void ) { return m_pTool_Light; }
	CTool_Interaction* Get_InteractionTool( void ) { return m_pTool_Interaction; }
private:
	CTool_Shader*			m_pTool_Shader = nullptr;
	CTool_Light*			m_pTool_Light = nullptr;
	CTool_Interaction*		m_pTool_Interaction = nullptr;
	vector<CPropertyPage*>	m_vecTool;
protected:
	DECLARE_MESSAGE_MAP()
};


