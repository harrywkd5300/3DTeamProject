#include "stdafx.h"
#include "MainApp.h"

#include "Component_Manager.h"
#include "Management.h"
#include "Scene_Logo.h"
#include "Scene_Video.h"
#include "Picking.h"
#include "Frame_Manager.h"
#include "Timer_Manager.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Subject_Manager.h" // By.PSH
#include "Random.h"
#include "Collider_Manager.h"
#include "Group_Manager.h"
#include "Decal_Manager.h"

USING(Engine)

CMainApp::CMainApp()
	: m_pGraphic_Device(Engine::CGraphic_Device::GetInstance())
	, m_pGraphicDev(nullptr)
	, m_pComponent_Manager(Engine::CComponent_Manager::GetInstance())
	, m_pManagement(Engine::CManagement::GetInstance())
{
	m_pGraphic_Device->AddRef();
	m_pComponent_Manager->AddRef();
	m_pManagement->AddRef();
}


CMainApp::~CMainApp()
{

}

HRESULT CMainApp::Ready_MainApp(void)
{
	if (FAILED(Ready_DefaultSetting(g_hWnd, CGraphic_Device::MODE_WIN, BACKSIZEX, BACKSIZEY)))
		return E_FAIL;

	if (FAILED(Ready_StaticComponent()))
		return E_FAIL;

	if (FAILED(Ready_StartScene()))
		return E_FAIL;
	
	return NOERROR;
}

int CMainApp::Update_MainApp(const float & fTimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

	m_fTimer += fTimeDelta;
	++m_iFps;

	return m_pManagement->Update_Management(fTimeDelta);	
}

void CMainApp::Render_MainApp(void)
{	
	if (nullptr == m_pGraphicDev
		|| nullptr == m_pManagement)
		return;

	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	m_pGraphicDev->BeginScene();
	
	// 그려야할객체들을 그린다.
#ifndef _FINAL
	CDebug::GetInstance()->BeginScene();
#endif
	
	m_pManagement->Render_Management();
	Render_FPS();
	Render_DeviceRef();

#ifndef _FINAL
	CDebug::GetInstance()->EndScene();
	CDebug::GetInstance()->Render_DebugList();
#endif

	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
}

// 내 게임을 초기화하기위해 필요한 사전작업을 수행하낟.
HRESULT CMainApp::Ready_DefaultSetting(HWND hWnd, Engine::CGraphic_Device::WINMODE eMode, const _uint& iBackCX, const _uint& iBackCY)
{
	if (nullptr == m_pGraphic_Device
		|| nullptr == m_pManagement)
		goto except;

	if (FAILED(m_pGraphic_Device->Ready_GraphicDev(&m_pGraphicDev, hWnd, eMode, iBackCX, iBackCY)))
		goto except;
	
	CDebug::GetInstance()->Set_GraphicDevice(m_pGraphicDev);


	//// GARA	
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if (FAILED(m_pManagement->Ready_Management(SCENE_END,m_pGraphicDev)))
		goto except;

	//CComponent* pCom = m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"CRenderer");
	//m_pManagement->SetUp_Renderer((CRenderer*)pCom);
	//CCamera_Manager::GetInstance()->SetUp_Renderer((CRenderer*)pCom);
	//Safe_Release(pCom);

	if ( FAILED( CDecal_Manager::GetInstance()->Ready_Decal_Manager( m_pGraphicDev ) ) )
		return E_FAIL;

	return NOERROR;

except:
	MSG_BOX("Ready_DefaultSetting Failed");
	return E_FAIL;
}

HRESULT CMainApp::Ready_StaticComponent()
{
	Engine::CComponent*		pComponent = nullptr;


	return NOERROR;
}

HRESULT CMainApp::Ready_StartScene(void)
{
	//if (nullptr == m_pManagement)
	//	return E_FAIL;

	//Engine::CScene*			pCurrentScene = nullptr;

	//pCurrentScene = CScene_Logo::Create(m_pGraphicDev,SCENETYPE::SCENE_LOGO);
	//if (nullptr == pCurrentScene)
	//	return E_FAIL;

	//if (FAILED(m_pManagement->SetUp_CurrentScene(pCurrentScene)))
	//	return E_FAIL;


	if (nullptr == m_pManagement)
		return E_FAIL;

	Engine::CScene*			pCurrentScene = nullptr;

	pCurrentScene = CScene_Logo::Create(m_pGraphicDev,SCENETYPE::SCENE_LOGO);

	if (nullptr == pCurrentScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_CurrentScene(pCurrentScene)))
		return E_FAIL;

	

	return NOERROR;
}

void CMainApp::Render_FPS()
{
	if (m_fTimer > 1.f)
	{
		m_iRenderFps = m_iFps;
		m_fTimer = 0.f;
		m_iFps = 0;
		
	}
#ifndef _FINAL
	CDebug::GetInstance()->Render_DebugText_Forced(10, 10, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), "FPS : %d", m_iRenderFps);
#else
	char Debug[32];
	sprintf(Debug, "FPS : %d", m_iRenderFps);
	CDebug::GetInstance()->Get_Font()->DrawTextW(10.f, 10.f, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), Debug);
#endif
}

void CMainApp::Render_DeviceRef()
{
	if (nullptr == m_pGraphicDev)
		return;
	CDebug::GetInstance()->Render_DebugText_Forced(10, 30, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), "DevRef : %d", m_pGraphicDev->AddRef());
	Safe_Release(m_pGraphicDev);
}

CMainApp * CMainApp::Create(void)
{
	CMainApp *	pInstance = new CMainApp();	
	
	if (FAILED(pInstance->Ready_MainApp()))
	{
		MessageBox(0, L"CMainApp Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);		
	}
		
	return pInstance;
}

unsigned long CMainApp::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pGraphic_Device);	
	Engine::Safe_Release(m_pComponent_Manager);
	Engine::Safe_Release(m_pManagement); // By.Jang	
	CDebug::GetInstance()->Release_CurDebug();
	CEffect_Manager::GetInstance()->Release_CurEffectList();

	_ulong	dwRefCnt = 0;	
	

	if (dwRefCnt = CGroupManager::GetInstance()->DestroyInstance())
		MSG_BOX("CGroupManager Released Failed");

	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		MSG_BOX("CManagement Released Failed");
		
	if (dwRefCnt = CObject_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CObject_Manager Released Failed");

	if (dwRefCnt = CEffect_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CEffect_Manager Released Failed");

	if (dwRefCnt = CCollider_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CCollider_Manager Released Failed");

	if ( dwRefCnt = CDecal_Manager::GetInstance()->DestroyInstance() )
		MSG_BOX( "CDecal_Manager Released Failed" );

	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CCamera_Manager Released Failed");

	if (dwRefCnt = CSubject_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CSubject_Manager Released Failed");

	if (dwRefCnt = CComponent_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CComponent_Manager Released Failed");

	if(dwRefCnt = CTarget_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CTarget_Manager Released Failed");

	if(dwRefCnt = CLight_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CLight_Manager Released Failed");

	//if (CDebug::GetInstance()->GetRefCnt() == 1)
	//	CDebug::GetInstance()->Release();
	//else
	//	MSG_BOX("CDebug Released Failed");

	if (dwRefCnt = CDebug::GetInstance()->DestroyInstance())
		MSG_BOX("CDebug Released Failed");

	if (dwRefCnt = CGraphic_Device::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphic_Device Released Failed");

	if (dwRefCnt = CFrame_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CFrame_Manager Released Failed");

	if (dwRefCnt = CTimer_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CTimer_Manager Released Failed");

	if (dwRefCnt = CInput_Device::GetInstance()->DestroyInstance())
		MSG_BOX("CInput_Device Released Failed");

	if (dwRefCnt = CAI_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CAI_Manager Released Failed");

	if (dwRefCnt = CRandom::GetInstance()->DestroyInstance())
		MSG_BOX("CRandom Released Failed");

	if (dwRefCnt = CString_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CString_ManagerReleased Failed");

	return dwRefCnt;
}
