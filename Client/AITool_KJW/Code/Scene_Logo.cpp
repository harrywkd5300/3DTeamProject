#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"

#include "Back_Logo.h"
#include "Component_manager.h"
#include "Camera_Dynamic.h"
#include "Scene_Stage.h"
#include "Management.h"
#include "Loading.h"
#include "Clinet_Component.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CScene_Logo", std::bind(&CScene_Logo::Debug_Render, this));
#endif
}

HRESULT CScene_Logo::Ready_Scene(void)
{
	CLoading* pLoading = nullptr;

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_BASIC_STAGE);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_SHADER);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_TEX_UI);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_Donatello);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_Leonardo);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_Michelangelo);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_Raphael);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_STAGE1_MONSTER);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	pLoading = CLoading::Create(m_pGraphicDev, LOAD_EFFECT);
	if (nullptr == pLoading)
		return E_FAIL;
	m_pLoadingList.push_back(pLoading);

	if (FAILED(Ready_LogoComponent()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera", &_vec3(0.f, 0.f, -5.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Write_CellInfo_Gara()))
		return E_FAIL;

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);


	return NOERROR;
}

_int CScene_Logo::Update_Scene(const _float & fTimeDelta)
{
	if (FAILED(Change_Scene(SCENE_STAGE)))
		return 0;

	bool bEnableNextStage = true;
	for (auto pLoading : m_pLoadingList)
	{
		if (false == pLoading->Get_Finish())
			bEnableNextStage = false;
	}

	if (bEnableNextStage)
		m_pLogo->Set_EnterKey(true);

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Logo::Render_Scene(void)
{
	Engine::CScene::Render_Scene();
}

HRESULT CScene_Logo::Ready_LogoComponent(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	//// For.Texture_BackLogo
	//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/BackGround_Logo.jpg");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_BackLogo", pComponent)))
	//	return E_FAIL;

	// For.Texture_BackLogo
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/BackGround_Logo.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;

	// For.Texture_KeyIcon
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/KeyBoard/Key_%d.png",48);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_KeyIcon", pComponent)))
		return E_FAIL;


	// For.Texture_KeyIcon
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/UpsellScreen.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_UpsellScreen", pComponent)))
		return E_FAIL;

	// For.Texture_KeyIcon
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Fade/Fade.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Fade", pComponent)))
		return E_FAIL;
	
	
	// For.Component_Shader_Default
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Default.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Shader_Default", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Default
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_UI.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Shader_UI", pComponent)))
		return E_FAIL;

	// for.Component_PlayerController
	pComponent = CPlayerController::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CPlayerController", pComponent)))
		return E_FAIL;

	// for.Component_AIController
	pComponent = CAIController::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CAIController", pComponent)))
		return E_FAIL;


	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Font.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CShader_Font", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Font/MarioFont.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_MarioFont", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Font/SeoulNamsanFont.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_SeoulNamsanFont", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Font/ZeldaFont.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_ZeldaFont", pComponent)))
		return E_FAIL;
	
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/ShuriKen_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_ShuriKen", pComponent)))
		return E_FAIL;
	

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_Camera(const _tchar * pLayerTag
			, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CCamera_Dynamic::Create(m_pGraphicDev, pEye, pAt, pUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	dynamic_cast<CCamera_Dynamic*>(pGameObject)->Set_Operation(true);

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_LOGO, pLayerTag, pGameObject)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Back_Logo
	pGameObject = CBack_Logo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_LOGO, pLayerTag, pGameObject)))
		return E_FAIL;

	m_pLogo = dynamic_cast<CBack_Logo*>(pGameObject);
	m_pLogo->AddRef();

	return NOERROR;
}

HRESULT CScene_Logo::Change_Scene(SCENETYPE eSceneID)
{
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_RETURN) )
	{
		for (auto pLoading : m_pLoadingList)
		{
			if (false == pLoading->Get_Finish())
				return NOERROR;
		}

 		Engine::CScene*			pNewScene = nullptr;

		switch (eSceneID)
		{
		case SCENE_STAGE:
			pNewScene = CScene_Stage::Create(m_pGraphicDev,SCENE_STAGE);
			if (nullptr == pNewScene)
				return E_FAIL;
			break;
		}

		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CScene_Logo::Write_CellInfo_Gara(void)
{
	HANDLE				hFile = 0;

	hFile = CreateFile(L"../Bin/Data/CellInfo.dat", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;

	_ulong		dwNumCell = 4;
	WriteFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, 0);

	_vec3		vPoint[3];

	vPoint[0] = _vec3(0.0f, 0.f, 100.f);
	vPoint[1] = _vec3(100.f, 0.f, 0.f);
	vPoint[2] = _vec3(0.0f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, 0);

	vPoint[0] = _vec3(0.0f, 0.f, 100.f);
	vPoint[1] = _vec3(100.f, 0.f, 100.f);
	vPoint[2] = _vec3(100.f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, 0);

	vPoint[0] = _vec3(100.f, 0.f, 100.f);
	vPoint[1] = _vec3(200.f, 0.f, 0.f);
	vPoint[2] = _vec3(100.f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, 0);

	vPoint[0] = _vec3(0.0f, 0.f, 200.f);
	vPoint[1] = _vec3(100.f, 0.f, 100.f);
	vPoint[2] = _vec3(0.0f, 0.f, 100.f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, 0);

	CloseHandle(hFile);

	return NOERROR;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx)
{
	CScene_Logo *	pInstance = new CScene_Logo(pGraphicDev);
	pInstance->m_iSceneIdx = iSceneIdx;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Logo Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

#ifndef _FINAL
void CScene_Logo::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CScene_Logo");

}
#endif
_ulong CScene_Logo::Free(void)
{

	Safe_Release(m_pLogo);

	if (FAILED(CScene::Release_Object(SCENE_LOGO)))	
		MSG_BOX("Scene_Logo Release Failed");		

	if(FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_LOGO)))
		MSG_BOX("Scene_Logo Release Failed");

	for (auto pLoading : m_pLoadingList)
	{
		Safe_Release(pLoading);
	}


	return Engine::CScene::Free();
}

