#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"

#include "Terrain.h"
#include "Player.h"
#include "Monster.h"
#include "Mous.h"
#include "KnifeThug.h"
#include "SlimThug.h"	// By.KMH
#include "Group_Manager.h"	// By.KMH
#include "Decal_Manager.h"	// By.KMH

#include "Sky.h"
#include "Tree.h"
#include "Component_manager.h"
#include "Camera_Dynamic.h"
#include "Effect_Sprite.h"
#include "UI.h"
#include "Stone.h"
#include "Light_Manager.h"
#include "EffectOBJ.h" // By.KJY
#include "Clinet_Component.h" // By.KJW

// Begin.PSH
#include "Camera_Player.h" 
#include "Camera_CCTV.h" 
#include "UI_Stats.h"
#include "PlayerChangeUI.h"
#include "ComboUI.h"
#include "Subject_Manager.h"
#include "ExpUI.h"
#include "ProgressUI.h"
#include "ParkourUI.h"
#include "NaviUI.h"
#include "NameTagUI.h"
#include "FadeUI.h"
#include "ScreenUI.h"
#include "PausedUI.h"
// End.PSH

#include "BaseObject.h"	// By.KOH
#include "MoveObject.h"	// By.KOH

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CScene_Stage", std::bind(&CScene_Stage::Debug_Render, this));
#endif

}

HRESULT CScene_Stage::Ready_Scene(void)
{
	if (FAILED(Ready_LightInfo()))
		return E_FAIL;

	if (FAILED(Ready_Decal()))
		return E_FAIL;
	
	//if (FAILED(Ready_Layer_CCTVCamera(L"Layer_CCTVCamera", &_vec3(193.f, 7.3f, 168.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_CCTVCamera(L"Layer_CCTVCamera", &_vec3(-5.f, 1.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
		return E_FAIL;	

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Player")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(L"Layer_Effect")))
	//	return E_FAIL;

	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;
	Engine::CComponent*		pComponent = nullptr;


#ifdef _PSH
#include "_PSHScene.cpp"
#endif

#ifdef _KJW
#include "_KJWScene.cpp"
#endif

//#ifdef _KJY
//#include "_KJYScene.cpp"
//#endif

#ifdef _KMH
#include "_KMHScene.cpp"
#endif

#ifdef _OSM
#include "_OSMScene.cpp"
#endif

//#ifdef _KOH
//#include "_KOHScene.cpp"
//#endif


	// 그룹 매니저를 통해 몬스터를 생성함

	if (FAILED(CGroupManager::GetInstance()->Ready_GroupManager(m_pGraphicDev, SCENE_STAGE)))
		return E_FAIL;


	return NOERROR;
}

_int CScene_Stage::Update_Scene(const _float & fTimeDelta)
{
	//if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_NUMPAD0))
	//{
	//	Engine::CGameObject*	pGameObject = nullptr;

	//	pGameObject = CMonster::Create(m_pGraphicDev);
	//	if (nullptr == pGameObject)
	//	{
	//		//return E_FAIL;
	//	}
	//	else
	//	{
	//		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
	//		{
	//			//return E_FAIL;
	//		}
	//	}
	//		
	//}

	CGroupManager::GetInstance()->Update_GroupManager( fTimeDelta );

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Stage::Render_Scene(void)
{
	Engine::CScene::Render_Scene();
}

HRESULT CScene_Stage::Ready_LightInfo(void)
{

	// ((빛.d * 재.d * 텍스쳐색) * 명암) + (빛.a * 재.a) + (빛.s * 재.s) * 스펙큘러의 세기
	D3DLIGHT9			LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));
	
	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	LightInfo.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	LightInfo.Direction = _vec3(1.f, -1.f, 1.f);
	
	if(FAILED(CLight_Manager::GetInstance()->Add_Light(m_pGraphicDev, L"Light_Default", &LightInfo)))
		return E_FAIL;

	//if ( FAILED( CLight_Manager::GetInstance()->LoadLightFile( m_pGraphicDev, L"../Bin/Data/LightData/LightData9.dat" ) ) )
	//	return E_FAIL;

	//LightInfo.Type = D3DLIGHT_POINT;
	//LightInfo.Diffuse = D3DXCOLOR( 248.f / 255.f, 188.f / 255.f, 80.f / 255.f, 1.f );
	//LightInfo.Ambient = D3DXCOLOR(0.0f, 0.f, 0.f, 1.f);
	//LightInfo.Specular = D3DXCOLOR( 0.f, 0.f, 0.f, 0.f );
	//LightInfo.Position = _vec3(90.f, 5.f, 100.f);
	//LightInfo.Range = 10.f;
	//
	//if(FAILED(CLight_Manager::GetInstance()->Add_Light(m_pGraphicDev, L"Light_Default", &LightInfo)))
	//	return E_FAIL;
	
	//LightInfo.Type = D3DLIGHT_POINT;
	//LightInfo.Diffuse = D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.f);
	//LightInfo.Ambient = D3DXCOLOR(0.1f, 0.3f, 0.1f, 1.f);
	//LightInfo.Specular = D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.f);
	//LightInfo.Position = _vec3(5.f, 5.f, 10.f);
	//LightInfo.Range = 10.f;
	//
	//if(FAILED(CLight_Manager::GetInstance()->Add_Light(m_pGraphicDev, L"Light_Default", &LightInfo)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Decal( void )
{
	SSDECALCREATEDATA	SSDecalData;

	SSDecalData.vPosition = _vec3( 200.f, 0.f, 200.f );
	SSDecalData.vRotationDegree = _vec3( 0.f, 0.f, 0.f );
	SSDecalData.vScale = _vec3( 50.f, 5.f, 50.f );
	lstrcpy( SSDecalData.szImageName, L"CTexture_Slot" );
	SSDecalData.iImageSceneIndex = SCENE_STATIC;

	if ( FAILED( CDecal_Manager::GetInstance()->Add_SSDecal( SSDecalData ) ) )
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_CCTVCamera(const _tchar * pLayerTag
			, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp)
{
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	Engine::CCamera*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CCamera_CCTV::Create(m_pGraphicDev, pEye, pAt, pUp);
	if (nullptr == pGameObject)
		return E_FAIL;	

	if (FAILED(m_pCamera_Manager->Add_Camera(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.Sky
	pGameObject = CSky::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	for (int i = 0; i < PLAYER_TYPE::PLAYER_None; ++i)
	{
		// For.Player
		pGameObject = CPlayer::Create(m_pGraphicDev, PLAYER_TYPE(i));
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
			return E_FAIL;
	}

	//// For.Player
	//pGameObject = CPlayer::Create( m_pGraphicDev, PLAYER_Donatello );
	//if ( nullptr == pGameObject )
	//	return E_FAIL;
	//if ( FAILED( m_pObject_Manager->Ready_GameObject( SCENE_STATIC, pLayerTag, pGameObject ) ) )
	//	return E_FAIL;
	
	//// For.Player
	//pGameObject = CPlayer::Create(m_pGraphicDev, CPlayer::PLAYER_TYPE::PLAYER_Donatello);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
	//	return E_FAIL;


	//// For.Player
	//pGameObject = CPlayer::Create(m_pGraphicDev, CPlayer::PLAYER_TYPE::PLAYER_Leonardo);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Player", pGameObject)))
	//	return E_FAIL;


	//// For.Player
	//pGameObject = CPlayer::Create(m_pGraphicDev, CPlayer::PLAYER_TYPE::PLAYER_Michelangelo);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Player", pGameObject)))
	//	return E_FAIL;


	//// For.Player
	//pGameObject = CPlayer::Create(m_pGraphicDev, CPlayer::PLAYER_TYPE::PLAYER_Raphael);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Player", pGameObject)))
	//	return E_FAIL;

	CPlayerController* pPlayerController = (CPlayerController*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CPlayerController");
	pPlayerController->Possess((CCharacter*)CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 0));
	//Safe_Release(pPlayerController);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	for (size_t i = 0; i < 10; ++i)
	{
		// For.Effect
		pGameObject = CEffect_Sprite::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	//if (nullptr == m_pObject_Manager)
	//	return E_FAIL;

	//Engine::CGameObject*	pGameObject = nullptr;
	//
	//// For.UI
	//pGameObject = CUI::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
	//	return E_FAIL;
	//
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_StaticObject(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Stone
	for (size_t i = 0; i < 1; i++)
	{
		pGameObject = CStone::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
			return E_FAIL;
	}

	// For.Tree
	for(size_t i = 0; i < 1; i++)
	{
		pGameObject = CTree::Create(m_pGraphicDev);
		if(nullptr == pGameObject)
			return E_FAIL;

		if(FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
			return E_FAIL;
	}
	

	return NOERROR;	
}


CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx)
{
	CScene_Stage *	pInstance = new CScene_Stage(pGraphicDev);
	pInstance->m_iSceneIdx = iSceneIdx;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Stage Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

#ifndef _FINAL
void CScene_Stage::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CScene_Stage");

}
#endif

_ulong CScene_Stage::Free(void)
{
	if (FAILED(CScene::Release_Object(SCENE_STAGE)))
		MSG_BOX("CScene_Stage Release Failed");

	if (FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_STAGE)))
		MSG_BOX("CScene_Stage Release Failed");
	
	CSubject_Manager::GetInstance()->Delete_DynamicData();

	return Engine::CScene::Free();
}

