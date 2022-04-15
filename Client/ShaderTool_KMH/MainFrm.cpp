
// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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

	//ModifyStyle( WS_CAPTION, NULL ); // Ÿ��Ʋ�� ���ֱ�

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("���� ������ ������ ���߽��ϴ�.\n");
	//	return -1;      // ������ ���߽��ϴ�.
	//}
	//
	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
	//	return -1;      // ������ ���߽��ϴ�.
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//
	//// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.hMenu = nullptr;

	return TRUE;
}

// CMainFrame ����

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


// CMainFrame �޽��� ó����



void CMainFrame::OnTimer( UINT_PTR nIDEvent )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CFrameWnd::OnTimer( nIDEvent );

	if ( nullptr == m_pMainView )
		return;

	m_pTimer_Manager->Compute_Timer( L"Timer_60" );
	_float fTimeDelta = m_pTimer_Manager->Get_TimeDelta( L"Timer_60" );

	m_pMainView->Update_MainView( fTimeDelta );
}
