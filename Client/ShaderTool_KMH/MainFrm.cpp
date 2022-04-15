
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"

#include "MainFrm.h"

#include "ShaderToolMainView.h"
#include "EditBox.h"

#include "Timer_Manager.h"
#include "Timer.h"

#include "Component_Manager.h"
#include "Management.h"
#include "Picking.h"
#include "Frame_Manager.h"
#include "Timer_Manager.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Subject_Manager.h" // By.PSH
#include "Random.h"
#include "Collider_Manager.h"
#include "Object_manager.h"
#include "Camera_Manager.h"
#include "Graphic_Device.h"
#include "Effect_Manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	CDebug::GetInstance()->Release_CurDebug();

	_ulong	dwRefCnt = 0;
	
	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CManagement Released Failed");
	
	if (dwRefCnt = CObject_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CObject_Manager Released Failed");
	
	if (dwRefCnt = CEffect_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CEffect_Manager not destroyed");
	
	if (dwRefCnt = CCollider_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CCollider_Manager Released Failed");
	
	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CCamera_Manager Released Failed");
	
	if (dwRefCnt = CSubject_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CSubject_Manager Released Failed");
	
	if (dwRefCnt = CComponent_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CComponent_Manager Released Failed");
	
	if(dwRefCnt = CTarget_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CTarget_Manager Released Failed");
	
	if(dwRefCnt = CLight_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CLight_Manager Released Failed");
	
	if (dwRefCnt = CDebug::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CDebug Released Failed");
	
	if (dwRefCnt = CGraphic_Device::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CGraphic_Device Released Failed");
	
	if (dwRefCnt = CFrame_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CFrame_Manager Released Failed");
	
	if (dwRefCnt = CTimer_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CTimer_Manager Released Failed");
	
	if (dwRefCnt = CInput_Device::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CInput_Device Released Failed");
	
	if (dwRefCnt = CAI_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CAI_Manager Released Failed");
	
	if (dwRefCnt = CRandom::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CRandom Released Failed");
	
	if (dwRefCnt = CString_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CString_ManagerReleased Failed");
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//ModifyStyle( WS_CAPTION, NULL ); // 타이틀바 없애기

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}
	//
	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//
	//// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	m_wndSplitter.CreateStatic( this, 1, 2 );

	//m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CMain_ToolView ), CSize( WINCX, WINCY ), pContext );
	//m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CEditBox ), CSize( TOOLCX, 300 ), pContext );

	m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CShaderToolMainView ), CSize( WINCX, WINCY ), pContext );
	m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CEditBox ), CSize( TOOLCX, 300 ), pContext );

	m_pEditBox = (CEditBox*)m_wndSplitter.GetPane( 0, 0 );
	m_pMainView = (CShaderToolMainView*)m_wndSplitter.GetPane( 0, 1 );

	SetTimer( NULL, (int)(1000.f / 60.f), NULL );

	m_pTimer_Manager = Engine::CTimer_Manager::GetInstance();
	if ( nullptr == m_pTimer_Manager )
		return FALSE;
	
	CTimer* pTimer = CTimer::Create();
	if ( nullptr == pTimer )
		return FALSE;
	if ( FAILED( m_pTimer_Manager->Add_Timer( L"Timer_60", pTimer ) ) )
		return FALSE;

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.hMenu = nullptr;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



void CMainFrame::OnTimer( UINT_PTR nIDEvent )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFrameWnd::OnTimer( nIDEvent );

	if ( nullptr == m_pMainView )
		return;

	m_pTimer_Manager->Compute_Timer( L"Timer_60" );
	_float fTimeDelta = m_pTimer_Manager->Get_TimeDelta( L"Timer_60" );

	m_pMainView->Update_MainView( fTimeDelta );
}
