#include "stdafx.h"
#include "..\Headers\Scene_Video.h"


#include "Clinet_Component.h"
#include "Scene_Logo.h"
#include "Component_manager.h"
#include "Management.h"


CScene_Video::CScene_Video(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CScene_Video", std::bind(&CScene_Video::Debug_Render, this));
#endif

}

HRESULT CScene_Video::Ready_Video(void)
{
	m_hVideo = MCIWndCreateW(g_hWnd, g_hInst, MCIWNDF_NOPLAYBAR | WS_VISIBLE | WS_CHILD
		, L"../Bin/Resources/Video/2.wmv");

	MoveWindow(m_hVideo, 0, 0, BACKSIZEX, BACKSIZEY, FALSE);

	MCIWndPlay(m_hVideo); // 미디어 재생

	return NOERROR;
}

HRESULT CScene_Video::Ready_Scene(void)
{
	if (FAILED(Ready_LogoComponent()))
		return E_FAIL;
				
	if (FAILED(Ready_Video()))
		return E_FAIL;

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);


	return NOERROR;
}

_int CScene_Video::Update_Scene(const _float & fTimeDelta)
{
	/*if (FAILED(Change_Scene(SCENE_LOGO)))
		return 0;*/
		
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_RETURN))
	{
		_int I = 0;
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Video::Render_Scene(void)
{
	Engine::CScene::Render_Scene();
}

HRESULT CScene_Video::Ready_LogoComponent(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;
	
	

	return NOERROR;
}

HRESULT CScene_Video::Change_Scene(SCENETYPE eSceneID)
{
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_RETURN))
	{
		Engine::CScene*			pNewScene = nullptr;

		switch (eSceneID)
		{
		case SCENE_LOGO:
			pNewScene = CScene_Logo::Create(m_pGraphicDev, SCENE_LOGO);
			if (nullptr == pNewScene)
				return E_FAIL;
			break;
		}

		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return E_FAIL;
	}
	return NOERROR;
}

CScene_Video * CScene_Video::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx)
{
	CScene_Video *	pInstance = new CScene_Video(pGraphicDev);
	pInstance->m_iSceneIdx = iSceneIdx;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Video Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

#ifndef _FINAL
void CScene_Video::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CScene_Video");

}
#endif
_ulong CScene_Video::Free(void)
{	
	MCIWndClose(m_hVideo);

	if (FAILED(CScene::Release_Object(SCENE_VIDEO)))
		MSG_BOX("Scene_Logo Release Failed");

	if (FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_VIDEO)))
		MSG_BOX("Scene_Logo Release Failed");

	return Engine::CScene::Free();
}

