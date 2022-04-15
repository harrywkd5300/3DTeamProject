#include "stdafx.h"
#include "..\Headers\Loading.h"

#include "Component_Manager.h"
#include "Clinet_Component.h"
#include "Effect_Manager.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

//  추가적인 스레드가 만들어지면  Thread_Main함수가 호출된다.
unsigned APIENTRY Thread_Main(void* pArgList)
{
	CLoading*	pLoading = (CLoading*)pArgList;

	EnterCriticalSection(pLoading->Get_Key());

	switch (pLoading->Get_LoadID())
	{
	case LOAD_SHADER:
		pLoading->Loading_Shader();
		break;
	case LOAD_TEX_UI:
		pLoading->Loading_Tex_UI();
		break;
	case LOAD_BASIC_STAGE:
		pLoading->Loading_ForStageScene();
		break;
	case LOAD_Donatello:
		pLoading->Loading_ForDonatello();
		break;
	case LOAD_Leonardo:
		pLoading->Loading_ForLeonardo();
		break;
	case LOAD_Michelangelo:
		pLoading->Loading_ForMichelangelo();
		break;
	case LOAD_Raphael:
		pLoading->Loading_ForRaphael();
		break;
	case LOAD_STAGE1_MONSTER:
		pLoading->Loading_ForStageScene_Monster();
		break;
	case LOAD_EFFECT:
		pLoading->Loading_ForEffect();
		break;
	}

	LeaveCriticalSection(pLoading->Get_Key());

	pLoading->Set_Finish(true);


	return 0;
}


HRESULT CLoading::Ready_Loading(LOADTYPE eSceneID)
{	
	m_eLoadID = eSceneID;

	InitializeCriticalSection(&m_Key);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;



	return NOERROR;
}

HRESULT CLoading::Loading_Shader(void)
{
	// For.Stage_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Shader_Terrain
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Terrain.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CShader_Terrain", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Skybox
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_SkyBox.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CShader_SkyBox", pComponent)))
		return E_FAIL;

	//// For.Component_Shader_Mesh
	//pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Mesh.fx");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(0, L"CShader_Mesh", pComponent)))
	//	return E_FAIL;

	// For.Component_Shader_Effect
	//pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Effect.fx");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CShader_Effect", pComponent)))
	//	return E_FAIL;

	// For.Component_Shader_Terrain_KOH
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/TerrainShader/Shader_Terrain.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CShader_Terrain_KOH", pComponent)))
		return E_FAIL;

	// For.Component_Shader_UI
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_UI.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CShader_UI", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Loading_Tex_UI(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Stats/Stats_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Stats", pComponent)))
		return E_FAIL;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Stats/Hp_%d.png", 3);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Hp", pComponent)))
		return E_FAIL;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Stats/Ep_%d.png", 3);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Ep", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Stats/StatsMask_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_StatsMask", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Swap/Swap_%d.png", 11);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Swap", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/ComboStats/Combo.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Combo", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Exp/Exp_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Exp", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Progress/Progress_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Progress", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Character/Don/Don_%d.png", 22);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Don", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Character/Leo/Leo_%d.png", 17);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Leo", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Character/Mike/Mike_%d.png", 26);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Mike", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Character/Raph/Raph_%d.png", 22);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Raph", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Ability/Ability_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Ability", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Item/Item_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Item", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Hint/Hint_%d.png", 3);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Hint", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Navi/Navi_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Navi", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/NameTag/NameTag_%d.png", 8);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_NameTag", pComponent)))
		return E_FAIL;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Stats/Slot_%d.png", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Slot", pComponent)))
		return E_FAIL;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Screen/Screen.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Screen", pComponent)))
		return E_FAIL;
	
	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Option/Option_%d.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_Option", pComponent)))
		return E_FAIL;

	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Option/Menu/Menu_%d.tga",4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_OptionMenu", pComponent)))
		return E_FAIL;


	// For.Texture
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Option/Option/Option_%d.tga", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_OptionCheck", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/UI/Option/Ability/Ability_%d.tga", 1);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CTexture_OptionAbility", pComponent)))
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CLoading::Loading_ForEffect(void)
{	
	Engine::CComponent*			pComponent = nullptr;
	
	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Test_Rsc.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Test2_Rsc.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Test3_Rsc.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Test_9999_Rsc.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Test_Mask_Rsc.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect_Resource(L"../Bin/Data/EffectData/Effect_Stage2_Wind_01_Rsc.dat")))
		return E_FAIL;


	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Test.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Test2.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Test3.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Test_9999.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Test_Mask.dat")))
		return E_FAIL;

	if (FAILED(CEffect_Manager::GetInstance()->Load_Effect(L"../Bin/Data/EffectData/Effect_Stage2_Wind_01.dat")))
		return E_FAIL;


	return NOERROR;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADTYPE eLoadID)
{
	CLoading *	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoadID)))
	{
		MessageBox(0, L"CLoading Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLoading::Loading_ForStageScene(void)
{
	// For.Stage_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	lstrcpy(m_szLoading, L"머티레얼 로딩중..");
	// For.Material
	pComponent = Engine::CMaterial::Create(m_pGraphicDev, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 20);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMaterial", pComponent)))	
		return E_FAIL;


	lstrcpy(m_szLoading, L"텍스쳐 로딩중....");

	// For.MultiTexture_Terrain
	pComponent = Engine::CMultiTexture::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Terrain/City/", L"Floor_Assphalt_4.mat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMultiTexture_Terrain", pComponent)))
		return E_FAIL;


	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Grass_%d.tga", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Terrain", pComponent)))
		return E_FAIL;


	// For.Texture_Sky
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_CUBE, L"../Bin/Resources/Textures/SkyBox/asd3.dds");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Sky", pComponent)))
		return E_FAIL;


	// For.Texture_Filter
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Filter.bmp");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Filter", pComponent)))
		return E_FAIL;

	// For.Texture_Brush
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Brush.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Brush", pComponent)))
		return E_FAIL;


	lstrcpy(m_szLoading, L"버퍼 로딩중......");

	// For.Buffer_Terrain
	pComponent = Engine::CTerrain_Buffer::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Terrain/Height.bmp", 1.f);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTerrain_Buffer", pComponent)))
		return E_FAIL;

	// For.Buffer_CubeTexture
	pComponent = Engine::CCube_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CCube_Texture", pComponent)))
		return E_FAIL;

	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CCollider", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"메시 로딩중........");


	lstrcpy(m_szLoading, L"네비게이션 메시를 생성 중........");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/CellInfo.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CNavigation", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"이펙트 셰이더를 생성 중........");




#ifdef _KJW
#include "_KJWLoading.cpp"
#endif

#ifdef _KJY
#include "_KJYLoading.cpp"
#endif

#ifdef _KMH
#include "_KMHLoading.cpp"
#endif

#ifdef _KOH
#include "_KOHLoading.cpp"
#endif

#ifdef _OSM
#include "_OSMLoading.cpp"
#endif

#ifdef _PSH
#include "_PSHLoading.cpp"
#endif


	lstrcpy(m_szLoading, L"여러분들에게 실망안하는중........");

	return NOERROR;
}

HRESULT CLoading::Loading_ForDonatello(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Mesh_Player
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Donatello/", L"Donatello.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Donatello", pComponent)))
		return E_FAIL;

	// For.Mesh_Staff
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Staff/", L"Staff.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Staff", pComponent)))
		return E_FAIL;

	//// For.Component_Mesh_Player
	//pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Leonardo/", L"Leonardo.X", _vec3(270.f, 180.f, 0.f));
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Leonardo", pComponent)))
	//	return E_FAIL;

	//// For.Component_Mesh_Player
	//pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Michelangelo/", L"Michelangelo.X", _vec3(270.f, 180.f, 0.f));
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Michelangelo", pComponent)))
	//	return E_FAIL;

	//// For.Component_Mesh_Player
	//pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Raphael/", L"Raphael.X", _vec3(270.f, 180.f, 0.f));
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Raphael", pComponent)))
	//	return E_FAIL;


	return NOERROR;
}

HRESULT CLoading::Loading_ForLeonardo(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Mesh_Player
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Leonardo/", L"Leonardo.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Leonardo", pComponent)))
		return E_FAIL;

	// For.Mesh_Sword
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Sword/", L"Sword.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Sword", pComponent)))
		return E_FAIL;




	return NOERROR;
}

HRESULT CLoading::Loading_ForMichelangelo(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Mesh_Player
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Michelangelo/", L"Michelangelo.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Michelangelo", pComponent)))
		return E_FAIL;

	// For.Mesh_Chucks
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Chucks/", L"Chucks_Static.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Chucks", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Loading_ForRaphael(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Mesh_Player
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Raphael/", L"Raphael.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Dynamic_Raphael", pComponent)))
		return E_FAIL;

	// For.Mesh_Sai
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Sai/", L"Sai.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Sai", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Loading_ForStageScene_Monster(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Component_Mesh_Mous
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/Mous/", L"Mous.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_Mous", pComponent)))
		return E_FAIL;

	// For.Component_Mesh_KnifeThug
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/KnifeThug/", L"KnifeThug.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_KnifeThug", pComponent)))
		return E_FAIL;

	// For.Component_Mesh_SlimThug
	pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/SlimThug/", L"SlimThug.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_SlimThug", pComponent)))
		return E_FAIL;

	// For.Mesh_Knife
	pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/Knife/", L"Knife.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Knife", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Loading_ForBossScene(void)
{
	Engine::CComponent*			pComponent = nullptr;


	return NOERROR;
}

_ulong CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pGraphicDev);

	DeleteCriticalSection(&m_Key);

	CloseHandle(m_hThread);

	return _ulong();
}
