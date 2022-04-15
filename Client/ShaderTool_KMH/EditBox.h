#pragma once
#include "Tool_Defines.h"
// CEditBox 폼 뷰입니다.

class CEditSheet;
class CEditBox : public CFormView
{
	DECLARE_DYNCREATE(CEditBox)

protected:
	CEditBox();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


