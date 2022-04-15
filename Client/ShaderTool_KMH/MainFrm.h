
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include "Tool_Defines.h"

namespace Engine
{
	class CTimer_Manager;
}

class CEditBox;
class CShaderToolMainView;
class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	CEditBox* Get_EditBox() { return m_pEditBox; }
	CShaderToolMainView* Get_MainView() { return m_pMainView; }
private:
	CEditBox*				m_pEditBox = nullptr;
	CShaderToolMainView*	m_pMainView = nullptr;
private:
	Engine::CTimer_Manager*	m_pTimer_Manager = nullptr;

// Ư���Դϴ�.
protected:
	CSplitterWnd m_wndSplitter;
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer( UINT_PTR nIDEvent );
};


