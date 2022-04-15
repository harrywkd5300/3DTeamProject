#pragma once
#include "Tool_Defines.h"
// CEditBox �� ���Դϴ�.

class CEditSheet;
class CEditBox : public CFormView
{
	DECLARE_DYNCREATE(CEditBox)

protected:
	CEditBox();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CEditBox();
public:
	CEditSheet* Get_EditSheet() { return m_pEditSheet; }
private:
	CEditSheet*	m_pEditSheet;
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITBOX };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


