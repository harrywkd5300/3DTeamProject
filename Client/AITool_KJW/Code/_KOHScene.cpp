

//// Stage1 
HANDLE hFile = CreateFile(L"../Bin/Data/Stage0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
	, FILE_ATTRIBUTE_NORMAL, NULL);

// Stage2
//HANDLE hFile = CreateFile(L"../Bin/Data/Stage2_1.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
//	, FILE_ATTRIBUTE_NORMAL, NULL);

// Stage3
//HANDLE hFile = CreateFile(L"../Bin/Data/Stage3_0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
//	, FILE_ATTRIBUTE_NORMAL, NULL);

//// BossStage
//HANDLE hFile = CreateFile(L"../Bin/Data/StageBoss_0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
//	, FILE_ATTRIBUTE_NORMAL, NULL);

if (INVALID_HANDLE_VALUE == hFile)
{
	MSG_BOX("Load Failed!!");
	return E_FAIL;
}

DWORD dwByte = 0;

_ulong dwNumObject = 0;

ReadFile(hFile, &dwNumObject, sizeof(_ulong), &dwByte, nullptr);

MAPOBJECT* pObjectData = new MAPOBJECT;

///////////////////////////////////////////////////////////////////////
// Add By.KMH - 렌더러에 계속 저장안하게 씬 불러올 때만 저장..
///////////////////////////////////////////////////////////////////////
CRenderer::STATICRENDERGROUP	StaticRenderGroup;
_ulong							dwTotalCount = dwNumObject;
StaticRenderGroup.iGroupCount = dwTotalCount;
StaticRenderGroup.pGroup = new CRenderer::STATICRENDEROBJECT[dwTotalCount];

for (_uint i = 0; i < dwNumObject; ++i)
{
	//ZeroMemory(pObjectData, sizeof(MAPOBJECT));

	ReadFile(hFile, pObjectData, sizeof(MAPOBJECT), &dwByte, nullptr);

	CComponent* pComponent_Find = CComponent_Manager::GetInstance()->Find_Component(SCENE_STAGE, pObjectData->ComName);
	if (nullptr == pComponent_Find)
	{
		// For.Mesh_Tree
		pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, pObjectData->FullPath, pObjectData->FileName);
		if (nullptr == pComponent)
			return E_FAIL;
		if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, CString_Manager::GetInstance()->GetStr(pObjectData->ComName), pComponent)))
			return E_FAIL;
	}

	if (!lstrcmp(pObjectData->FileName, L"Tunnel_segment_Long.X") ||
		!lstrcmp(pObjectData->FileName, L"Light_Red.X") || 
		!lstrcmp(pObjectData->FileName, L"Light_Yellow.X") ||
		!lstrcmp(pObjectData->FileName, L"Sub_Traincar.X"))
	{
		pGameObject = CMoveObject::Create(m_pGraphicDev, *pObjectData, true, nullptr);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_MoveObject", pGameObject, OBJ_TYPE_DYNAMIC)))
			return E_FAIL;

		StaticRenderGroup.pGroup[i].fCullSize = ((CMoveObject*)pGameObject)->Get_CullSize();
	}
	else
	{
		pGameObject = CBaseObject::Create(m_pGraphicDev, *pObjectData, true, nullptr);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_BaseObject", pGameObject, OBJ_TYPE_STATIC)))
			return E_FAIL;

		StaticRenderGroup.pGroup[i].fCullSize = ((CBaseObject*)pGameObject)->Get_CullSize();
	}

	// Add By.KMH - 생성된 게임 오브젝트 구조체에 저장..
	StaticRenderGroup.pGroup[i].pGameObject = pGameObject;
	StaticRenderGroup.pGroup[i].fSort = 0.f;
}

ReadFile(hFile, &dwNumObject, sizeof(_uint), &dwByte, nullptr);

//MAPOBJECT* pObjectData = new MAPOBJECT;
while (true)
{
	ZeroMemory(pObjectData, sizeof(MAPOBJECT));

	_uint dwComObject = 0;

	ReadFile(hFile, &dwComObject, sizeof(_uint), &dwByte, nullptr);

	_tchar tComKey[128] = L"";

	ReadFile(hFile, &tComKey, sizeof(_tchar) * 128, &dwByte, nullptr);

	_uint iSize = 0;

	if (dwNumObject != iSize)
	{
		_uint iFirst = 0;

		// Add By.KMH - 두 번째 루프를 돌기 전 더 큰 공간을 생성하고 거기에 옮기기..
		_ulong dwTemp = dwTotalCount;
		dwTotalCount += dwComObject;
		CRenderer::STATICRENDERGROUP	Tmp;
		Tmp.pGroup = new CRenderer::STATICRENDEROBJECT[dwTotalCount];
		Tmp.iGroupCount = dwTotalCount;

		for ( _uint i = 0; i < dwTemp; ++i )
		{
			Tmp.pGroup[i].pGameObject = StaticRenderGroup.pGroup[i].pGameObject;
			Tmp.pGroup[i].fSort = StaticRenderGroup.pGroup[i].fSort;
			Tmp.pGroup[i].fCullSize = StaticRenderGroup.pGroup[i].fCullSize;
		}

		Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
		StaticRenderGroup = Tmp;

		for (_uint i  = 0; i < dwComObject; ++i)
		{
			_matrix matWorld;
			ReadFile(hFile, matWorld, sizeof(_matrix), &dwByte, nullptr);

			ZeroMemory(pObjectData, sizeof(MAPOBJECT));
			ReadFile(hFile, pObjectData, sizeof(MAPOBJECT), &dwByte, nullptr);

			CComponent* pComponent_Find = CComponent_Manager::GetInstance()->Find_Component(SCENE_STAGE, pObjectData->ComName);
			if (nullptr == pComponent_Find)
			{
				// For.Mesh_Tree
				pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, pObjectData->FullPath, pObjectData->FileName);
				if (nullptr == pComponent)
					return E_FAIL;
				if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, CString_Manager::GetInstance()->GetStr(pObjectData->ComName), pComponent)))
					return E_FAIL;
			}

			if (!lstrcmp(pObjectData->FileName, L"Tunnel_segment_Long.X") ||
				!lstrcmp(pObjectData->FileName, L"Light_Red.X") ||
				!lstrcmp(pObjectData->FileName, L"Light_Yellow.X")|| 
				!lstrcmp(pObjectData->FileName, L"Sub_Traincar.X"))
			{
				pGameObject = CMoveObject::Create(m_pGraphicDev, *pObjectData, false, &matWorld);
				if (nullptr == pGameObject)
					return E_FAIL;

				if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_MoveObject", pGameObject, OBJ_TYPE_DYNAMIC)))
					return E_FAIL;

				StaticRenderGroup.pGroup[dwTemp + i].fCullSize = ((CMoveObject*)pGameObject)->Get_CullSize();
			}
			else
			{
				pGameObject = CBaseObject::Create(m_pGraphicDev, *pObjectData, false, &matWorld);
				if (nullptr == pGameObject)
					return E_FAIL;

				if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_BaseObject", pGameObject, OBJ_TYPE_STATIC)))
					return E_FAIL;

				StaticRenderGroup.pGroup[dwTemp + i].fCullSize = ((CBaseObject*)pGameObject)->Get_CullSize();
			}

			// Add By.KMH - 생성된 게임 오브젝트 구조체에 저장..
			StaticRenderGroup.pGroup[dwTemp + i].pGameObject = pGameObject;
			StaticRenderGroup.pGroup[dwTemp + i].fSort = 0.f;

			//pGameObject = CBaseObject::Create(m_pGraphicDev, *pObjectData, false, &matWorld);

			//if (nullptr == pGameObject)
			//	return E_FAIL;

			//if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_BaseObject", pGameObject)))
			//	return E_FAIL;

			//// Add By.KMH - 생성된 게임 오브젝트 구조체에 저장..
			//StaticRenderGroup.pGroup[dwTemp + i].pGameObject = pGameObject;
			//StaticRenderGroup.pGroup[dwTemp + i].fSort = 0.f;
			//StaticRenderGroup.pGroup[dwTemp + i].fCullSize = ((CBaseObject*)pGameObject)->Get_CullSize();

			++iFirst;
		}
	}

	if (0 == dwByte)
	{
		//Engine::Safe_Delete(pObjectData);
		break;
	}

	++iSize;
}

Engine::Safe_Delete(pObjectData);
CloseHandle(hFile);

// Add By.KMH - 렌더러에 최종적으로 저장..
CRenderer* pRenderer = dynamic_cast<CRenderer*>(CComponent_Manager::GetInstance()->Clone_Component( SCENE_STATIC, L"CRenderer" ));
if ( nullptr == pRenderer )
{
	Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
	return E_FAIL;
}
if ( FAILED( pRenderer->Add_StaticRenderGroup( CRenderer::STATICRENDER_NONALPHA, StaticRenderGroup ) ) )
{
	Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
	return E_FAIL;
}
Engine::Safe_Release( pRenderer );