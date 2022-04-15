//////////////////////3D UI 생성(Debug용)///////////////////////////////
pGameObject = CUI_Stats::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;


pGameObject = CPlayerChangeUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;

//////////////////////3D UI 생성(Debug용)///////////////////////////////


//////////////////////2D UI 생성(Debug용)///////////////////////////////
pGameObject = CComboUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;

pGameObject = CExpUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;

pGameObject = CProgressUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject)))
return E_FAIL;









////// 파쿠르 ////////////////////


HANDLE hParkourFile = CreateFile(L"../Bin/Data/UIData/Parkour_Point.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
	, FILE_ATTRIBUTE_NORMAL, NULL);

if (INVALID_HANDLE_VALUE == hParkourFile)
{
	MSG_BOX("Load Failed!!");
	return E_FAIL;
}

DWORD dwParkOurByte = 0;

_uint iParkourSize = 0;

ReadFile(hParkourFile, &iParkourSize, sizeof(_ulong), &dwParkOurByte, nullptr);

for (_uint i = 0; i < iParkourSize; i++)
{
	_vec3 vPos;
	_vec3 vAngle;

	ReadFile(hParkourFile, &vPos, sizeof(_vec3), &dwParkOurByte, nullptr);
	ReadFile(hParkourFile, &vAngle, sizeof(_vec3), &dwParkOurByte, nullptr);
		
	pGameObject = CParkourUI::Create(m_pGraphicDev,vPos,vAngle);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject)))
		return E_FAIL;
	
	if (0 == dwParkOurByte)
	{
		break;
	}

}
CloseHandle(hParkourFile);



/////////////////////파쿠르////////


/////////////////// 네비 불러오기



//// Stage1 


pGameObject = CNaviUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject)))
return E_FAIL;

HANDLE hNaviFile = CreateFile(L"../Bin/Data/UIData/Navi_Point.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
	, FILE_ATTRIBUTE_NORMAL, NULL);

if (INVALID_HANDLE_VALUE == hNaviFile)
{
	MSG_BOX("Load Failed!!");
	return E_FAIL;
}

DWORD dwNaviByte = 0;

_uint iSize = 0;

ReadFile(hNaviFile, &iSize, sizeof(_ulong), &dwNaviByte, nullptr);

dynamic_cast<CNaviUI*>(pGameObject)->Set_NaviContainerSize(iSize);

for (_uint i = 0; i < iSize; i++)
{
	_vec3 vPos;

	ReadFile(hNaviFile, &vPos, sizeof(_vec3), &dwNaviByte, nullptr);

	dynamic_cast<CNaviUI*>(pGameObject)->Add_NaviPoint(vPos);	

	if (0 == dwNaviByte)
	{
		break;
	}

}
CloseHandle(hNaviFile);


/////////////////// 네비 불러오기
for (_int i = 0; i < 4; i++)
{
	pGameObject = CNameTagUI::Create(m_pGraphicDev,(PLAYER_TYPE)(i));

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
		return E_FAIL;
}


pGameObject = CFadeUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;

pGameObject = CScreenUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;


pGameObject = CPausedUI::Create(m_pGraphicDev);

if (nullptr == pGameObject)
return E_FAIL;

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, L"Layer_UI", pGameObject)))
return E_FAIL;

//////////////////////2D UI 생성(Debug용)///////////////////////////////
