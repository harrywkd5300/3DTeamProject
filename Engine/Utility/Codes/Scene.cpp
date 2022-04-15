#include "..\Headers\Scene.h"
#include "Collider_Manager.h" // By.KMH


USING(Engine)

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pCamera_Manager(CCamera_Manager::GetInstance())  
	, m_pCollider_Manager( CCollider_Manager::GetInstance() )
	, m_pEffect_Manager(CEffect_Manager::GetInstance())
{
	m_pGraphicDev->AddRef();
	m_pObject_Manager->AddRef();
	m_pCamera_Manager->AddRef(); 
	m_pCollider_Manager->AddRef();
	m_pEffect_Manager->AddRef();
#ifndef _FINAL
	Tag(L"CScene", std::bind(&CScene::Debug_Render, this));
#endif

}

HRESULT CScene::Ready_Scene(void)
{

	return NOERROR;
}

_int CScene::Update_Scene(const _float & fTimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	_int		iExitCode = 0;

	iExitCode = m_pCamera_Manager->Update_Camera_Manager(fTimeDelta);

	iExitCode = m_pObject_Manager->Update_Object_Manager(fTimeDelta);

	m_pEffect_Manager->Update(fTimeDelta);
	
	// 오브젝트매니저의 ExitCode로 수행 할 일이 있다면 이부분에 처리  
	

	return iExitCode;
}

_int CScene::LastUpdate_Scene(const _float & fTimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	_int		iExitCode = 0;

	iExitCode = m_pCamera_Manager->LastUpdate_Camera_Manager(fTimeDelta);

	iExitCode = m_pObject_Manager->LastUpdate_Object_Manager(fTimeDelta);

	m_pEffect_Manager->LastUpdate(fTimeDelta);

	m_pCollider_Manager->Update_Collider_Manager();	// By.KMH - 콜라이더 매니저 업데이트 돌면서 충돌 체크..

	// 오브젝트매니저의 ExitCode로 수행 할 일이 있다면 이부분에 처리  

	

	return iExitCode;
}

void CScene::Render_Scene(void)
{
	
}

HRESULT CScene::Release_Object(const _uint & iSceneIdx)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pCamera_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Release_Object(iSceneIdx)))
		return E_FAIL;

	return m_pCamera_Manager->Release_Camera(iSceneIdx); 

}
#ifndef _FINAL
void CScene::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CScene");
}
#endif
_ulong CScene::Free(void)
{
	_ulong dwRefCnt = 0;

	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pCamera_Manager);
	Engine::Safe_Release(m_pObject_Manager); //By.Jangwon
	Engine::Safe_Release( m_pCollider_Manager ); // By.KMH
	Engine::Safe_Release(m_pEffect_Manager); // By.KJW
	

	return dwRefCnt;
}
