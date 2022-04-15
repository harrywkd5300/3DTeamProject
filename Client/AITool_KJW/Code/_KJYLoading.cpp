//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Sprite/FX_FlameFlip_006_TEX_HKB.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Sprite_Test", pComponent)))
//return E_FAIL;
//
////pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/AlphaMaskAni/FX_NoiseTile_006_TEX_KKJ.tga");
////if (nullptr == pComponent)
////return E_FAIL;
////if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test1", pComponent)))
////return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/AlphaMaskAni/FX_Flake_001_TEX_KKJ.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test1", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/AlphaMaskAni/FX_Flake_001_TEX_Alpha.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test3", pComponent)))
//return E_FAIL;
//
////pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/AlphaMask/FX_BoomDust01.tga");
////if (nullptr == pComponent)
////return E_FAIL;
////if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test2", pComponent)))
////return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/AlphaMask/FX_Energy_dust.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test2", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Tiling/FX_fire_sheet.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Tiling_Test1", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Tiling/fx_tilingFire.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Tiling_Test2", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Normal/T_AM_sb_LF_M.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Mesh_Test1", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Particle/A_GlowBlue001_emis.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Particle_Test1", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/DistortNormal/heatwave_normalbuffer.png");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Distort_Test1", pComponent)))
//return E_FAIL;
//
//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/DistortNormal/t_vfx_distortionSphere_01.tga");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CTexture_Distort_Test2", pComponent)))
//return E_FAIL;

// Mesh
//pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Effect/", L"sm_vfx_lightningBolt_01.X");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Static_Effect_Test1", pComponent)))
//return E_FAIL;

//pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Effect/", L"sm_vfx_groundWave.X");
//if (nullptr == pComponent)
//return E_FAIL;
//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Static_Effect_Test1", pComponent)))
//return E_FAIL;

// --------------------------------------------------------------------------------------------------------------------------------------//
// 로드할 이펙트 리소스 불러오기

//HANDLE		hFile_Rsc = CreateFile(L"../Bin/Data/EffectData/Effect_Test_Rsc.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//if (INVALID_HANDLE_VALUE == hFile_Rsc)
//{
//	MSG_BOX("Effect Resource Load Failed!!");
//	return E_FAIL;
//}
//
//_ulong		dwByte = 0;
//
//pComponent = nullptr;
//
//TCHAR	szComponentTag[MAX_PATH] = L"";
//TCHAR	szFilePath[MAX_PATH] = L"";
//
//_uint	iListCnt = 0, iMeshCnt = 0, iTexCnt = 0, iLength = 0;
//
//ReadFile(hFile_Rsc, &iListCnt, sizeof(_uint), &dwByte, nullptr);
//
//for (_uint i = 0; i < iListCnt; ++i)
//{
//	ReadFile(hFile_Rsc, &iMeshCnt, sizeof(_uint), &dwByte, nullptr);
//	ReadFile(hFile_Rsc, &iTexCnt, sizeof(_uint), &dwByte, nullptr);
//
//	for (size_t i = 0; i < iMeshCnt; ++i)
//	{
//		ReadFile(hFile_Rsc, &iLength, sizeof(_uint), &dwByte, nullptr);
//		if (0 == dwByte)
//			break;
//		ReadFile(hFile_Rsc, szComponentTag, sizeof(TCHAR) * iLength, &dwByte, nullptr);
//		ReadFile(hFile_Rsc, szFilePath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
//
//		Engine::CComponent* pComponent = CComponent_Manager::GetInstance()->Find_Component(SCENE_STATIC, szComponentTag);
//		if (nullptr == pComponent)
//		{
//			pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, szFilePath, szComponentTag);
//			if (nullptr == pComponent)
//				return E_FAIL;
//			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, CString_Manager::GetInstance()->GetStr(szComponentTag), pComponent)))
//				return E_FAIL;
//		}
//	}
//
//	for (size_t i = 0; i < iTexCnt; ++i)
//	{
//		ReadFile(hFile_Rsc, &iLength, sizeof(_uint), &dwByte, nullptr);
//		if (0 == dwByte)
//			break;
//		ReadFile(hFile_Rsc, szComponentTag, sizeof(TCHAR) * iLength, &dwByte, nullptr);
//		ReadFile(hFile_Rsc, szFilePath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
//
//		Engine::CComponent* pComponent = CComponent_Manager::GetInstance()->Find_Component(SCENE_STATIC, szComponentTag);
//		if (nullptr == pComponent)
//		{
//			pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, szFilePath);
//			if (nullptr == pComponent)
//				return E_FAIL;
//			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, CString_Manager::GetInstance()->GetStr(szComponentTag), pComponent)))
//				return E_FAIL; // 중복되는 리소스의 경우 그냥 넘어가도록?
//		}
//	}
//}