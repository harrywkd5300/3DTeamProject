
// MainFrm.h : CMainFrame 클래스의 인터페이스
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
	
protected: // serialization에서만 만들어집니다.
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

// 특성입니다.
protected:
	CSplitterWnd m_wndSplitter;
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer( UINT_PTR nIDEvent );
};


