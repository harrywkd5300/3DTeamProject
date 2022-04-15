// ../Codes/ShaderToolMainView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "ShaderToolMainView.h"

#include "MainFrm.h"
#include "Scene_Tool.h"
#include "EditBox.h"
#include "EditSheet.h"
#include "Tool_Shader.h"
#include "Tool_Light.h"
#include "Tool_Interaction.h"

#include "Graphic_Device.h"
#include "Component_Manager.h"
#include "Management.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Collider_Manager.h"

HWND g_hWnd;
// CShaderToolMainView

IMPLEMENT_DYNCREATE(CShaderToolMainView, CView)

CShaderToolMainView::CShaderToolMainView()
	: m_pObject_Manager( CObject_Manager::GetInstance() )
	, m_pGraphic_Device( CGraphic_Device::GetInstance() )
	, m_pComponent_Manager( CComponent_Manager::GetInstance() )
	, m_pManagement( CManagement::GetInstance() )
{

}

CShaderToolMainView::~CShaderToolMainView()
{
	Engine::Safe_Release( m_pGraphicDev );
}

void CShaderToolMainView::Update_MainView( const _float & _fTimeDelta )
{
	if ( nullptr == m_pGraphicDev ||
		 nullptr == m_pManagement )
		return;

	POINT ptMouse;
	GetCursorPos( &ptMouse );

	ScreenToClient( &ptMouse );

	if ( m_ptPrevMouse.x == 0 && m_ptPrevMouse.y == 0 )
		m_ptPrevMouse = ptMouse;

	_uint iRange = 10;

	if ( ptMouse.x <= iRange || ptMouse.x >= WINCX - iRange || ptMouse.y <= iRange || ptMouse.y >= WINCY - iRange )
	{
		m_bMouseInClient = false;
		m_ptMove.x = m_ptMove.y = 0;
	}
	else
	{
		if ( m_bMouseInClient == false )
			m_ptPrevMouse = ptMouse;
		m_bMouseInClient = true;
	}

	if ( m_bMouseInClient == true )
	{
		m_ptMove.x = ptMouse.x - m_ptPrevMouse.x;
		m_ptMove.y = ptMouse.y - m_ptPrevMouse.y;

		m_ptPrevMouse = ptMouse;
	}

	CInput_Device::GetInstance()->Check_InputState();

	CEditBox* pEditBox = ((CMainFrame*)AfxGetMainWnd())->Get_EditBox();
	if ( nullptr == pEditBox )
		return;
	CEditSheet* pEditSheet = pEditBox->Get_EditSheet();
	if ( nullptr == pEditSheet )
		return;

	CTool_Shader* pShaderTool = pEditSheet->Get_ShaderTool();
	CTool_Light* pLightTool = pEditSheet->Get_LightTool();
	CTool_Interaction* pInteractionTool = pEditSheet->Get_InteractionTool();

	if ( nullptr == pShaderTool ||
		 nullptr == pLightTool ||
		 nullptr == pInteractionTool )
		return;

	if ( pShaderTool->bIsActive )
		pShaderTool->Update_Tool( _fTimeDelta );
	else if ( pLightTool->bIsActive )
		pLightTool->Update_Tool( _fTimeDelta );
	else if ( pInteractionTool->bIsActive )
		pInteractionTool->Update_Tool( _fTimeDelta );

	/* ================================ Update Scene.. ================================ */
	m_pManagement->Update_Management( _fTimeDelta );


	/* ================================ Render Scene.. ================================ */
	m_pGraphicDev->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR( 0.f, 0.f, 1.f, 1.f ), 1.f, 0 );
	m_pGraphicDev->BeginScene();


	m_pManagement->Render_Management();


	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present( nullptr, nullptr, NULL, nullptr );
}

BEGIN_MESSAGE_MAP(CShaderToolMainView, CView)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CShaderToolMainView 그리기입니다.

HRESULT CShaderToolMainView::Ready_DefaultSetting( void )
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	RECT rcWindow;

	pMainFrame->GetWindowRect(&rcWindow);

	SetRect(&rcWindow, 0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);

	RECT rcMainView;

	GetClientRect(&rcMainView);

	float fX = float(rcWindow.right - rcMainView.right);

	float fY = float(rcWindow.bottom - rcMainView.bottom);

	pMainFrame->SetWindowPos(NULL, 0, 0, int(WINCX + fX), int(WINCY + fY), SWP_NOZORDER);

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	g_hWnd = m_hWnd;

	if ( nullptr == m_pGraphic_Device )
		return E_FAIL;

	HINSTANCE hInst = AfxGetInstanceHandle();

	if ( FAILED( m_pGraphic_Device->Ready_GraphicDev( &m_pGraphicDev, g_hWnd, Engine::CGraphic_Device::MODE_WIN, WINCX, WINCY ) ) )
		return E_FAIL;
	if ( FAILED( CInput_Device::GetInstance()->Ready_Input_Device( hInst, ((CMainFrame*)AfxGetMainWnd())->GetSafeHwnd() ) ) )
		return E_FAIL;

	D3DLIGHT9 lightInfo;
	ZeroMemory(&lightInfo, sizeof(D3DLIGHT9));

	lightInfo.Type = D3DLIGHT_DIRECTIONAL;
	lightInfo.Diffuse = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	lightInfo.Ambient = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	lightInfo.Specular = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	lightInfo.Direction = _vec3( 1.f, -1.f, 1.f );

	m_pGraphicDev->SetLight( 0, &lightInfo );
	m_pGraphicDev->LightEnable( 0, TRUE );

	return NOERROR;
}

HRESULT CShaderToolMainView::Ready_Management( void )
{
	if ( nullptr == m_pManagement )
		return E_FAIL;

	if ( FAILED( m_pManagement->Ready_Management( TOOLSCENE_END, m_pGraphicDev ) ) )
	{
		Engine::Safe_Release( m_pManagement );
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CShaderToolMainView::Ready_StaticComponent( void )
{
	Engine::CRenderer*	pRendererCom = nullptr;

	Engine::CComponent*	pComponent = nullptr;

	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Grass_%d.tga", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CTexture_Terrain", pComponent)))
		return E_FAIL;

	// For.Texture_Filter
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Filter.bmp");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CTexture_Filter", pComponent)))
		return E_FAIL;

	// For.Texture_Brush
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Brush.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CTexture_Brush", pComponent)))
		return E_FAIL;

	// For.Terrain_Buffer
	pComponent = Engine::CTerrain_Buffer::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Terrain/Height.bmp", 1.f);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CTerrain_Buffer", pComponent)))
		return E_FAIL;


	// For.Component_Shader_Terrain
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Terrain.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CShader_Terrain", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Skybox
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_SkyBox.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CShader_SkyBox", pComponent)))
		return E_FAIL;

	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CCollider", pComponent)))
		return E_FAIL;

	// For.Mesh_Staff
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Staff/", L"Staff.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CMesh_Static_Staff", pComponent)))
		return E_FAIL;

	// For.Mesh_Sword
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Sword/", L"Sword.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CMesh_Static_Sword", pComponent)))
		return E_FAIL;

	// For.Mesh_Chucks
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Chucks/", L"Chucks_Static.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CMesh_Static_Chucks", pComponent)))
		return E_FAIL;

	// For.Mesh_Sai
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Sai/", L"Sai.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CMesh_Static_Sai", pComponent)))
		return E_FAIL;

	// For.Mesh_Knife
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/Knife/", L"Knife.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, L"CMesh_Static_Knife", pComponent)))
		return E_FAIL;

	// CManagement Object SetUp Renderer Component..
	if ( nullptr == m_pManagement )
		return E_FAIL;

	pRendererCom = (CRenderer*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" );
	m_pManagement->SetUp_Renderer( pRendererCom );
	Engine::Safe_Release( pRendererCom );

	return NOERROR;
}

HRESULT CShaderToolMainView::Ready_Scene( void )
{
	CScene_Tool*	pScene = CScene_Tool::Create( m_pGraphicDev );
	if ( nullptr == pScene )
		return E_FAIL;
	if ( FAILED( m_pManagement->SetUp_CurrentScene( pScene ) ) )
		return E_FAIL;

	return NOERROR;
}

void CShaderToolMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CShaderToolMainView 진단입니다.

#ifdef _DEBUG
void CShaderToolMainView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CShaderToolMainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShaderToolMainView 메시지 처리기입니다.


void CShaderToolMainView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( FAILED( Ready_DefaultSetting() ) )
		goto Fail_Initialize;

	if ( FAILED( Ready_Management() ) )
		goto Fail_Initialize;

	if ( FAILED( Ready_StaticComponent() ) )
		goto Fail_Initialize;

	if ( FAILED( Ready_Scene() ) )
		goto Fail_Initialize;

	return;

Fail_Initialize:
	AfxMessageBox( L"CMain_ToolView OnInitialUpdate Failed!!" );
	PostQuitMessage( 0 );
}


void CShaderToolMainView::OnMouseMove( UINT nFlags, CPoint point )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_ptMouse = point;

	CView::OnMouseMove( nFlags, point );
}
