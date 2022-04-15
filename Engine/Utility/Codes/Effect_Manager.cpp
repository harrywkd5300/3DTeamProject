#include "..\Headers\Effect_Manager.h"
#include "Management.h"
#include "Camera_Manager.h"
#include "atlstr.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()

{
#ifndef _FINAL
	Tag(L"CEffect_Manager", std::bind(&CEffect_Manager::Debug_Render, this));
#endif

}

HRESULT CEffect_Manager::Init_Manager(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (pGraphicDev == nullptr)
		return E_FAIL;
	
	m_pGraphicDev = pGraphicDev;

	return NOERROR;
}

HRESULT CEffect_Manager::Load_Effect(const _tchar * pFileFullPath)
{
	if (pFileFullPath == nullptr)
		return E_FAIL;

	CEffectOBJ* pNewEffect = CEffectOBJ::Create(m_pGraphicDev);
	if (nullptr == pNewEffect)
		return E_FAIL;

	// 데이터를 로드 한다.
	HANDLE	hFile_Effect = CreateFile(pFileFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile_Effect == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_ulong		dwByte = 0;
	_ulong		dwNumComponents = 0;
	_uint		iLength, iListCnt = 0;
	_tchar*		pEffectTag = nullptr;

	EFFECT_INFO		tInfo = EFFECT_INFO();

	ReadFile(hFile_Effect, &iLength, sizeof(_uint), &dwByte, nullptr);
	pEffectTag = CString_Manager::GetInstance()->GetTchar(iLength);
	ReadFile(hFile_Effect, pEffectTag, sizeof(TCHAR) * iLength, &dwByte, nullptr);
	
	// 이펙트 태그 로드 (CString_Manager 쓸 것)
	pNewEffect->Set_EffectTag(pEffectTag);
	ReadFile(hFile_Effect, &iListCnt, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iListCnt; ++i)
	{
		ReadFile(hFile_Effect, &tInfo, sizeof(EFFECT_INFO), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		Engine::CMyEffect*	pEffectCom = nullptr;

		if (tInfo.m_ePassType == EFFECT_INFO::PASS_MASKING
			|| tInfo.m_ePassType == EFFECT_INFO::PASS_BUFFER
			|| tInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
		{
			pEffectCom = CMyEffect_Buff::Create(m_pGraphicDev, tInfo);
			pNewEffect->Add_Effect(pEffectCom);
		}
		else if (tInfo.m_ePassType == EFFECT_INFO::PASS_PARTICLE)
		{
			pEffectCom = CMyEffect_Particle::Create(m_pGraphicDev, tInfo);
			pNewEffect->Add_Effect(pEffectCom);
		}
	}

	auto iter_Origin = find_if(m_OriginEffectOBJMap.begin(), m_OriginEffectOBJMap.end(), [&](const ORIGIN_EFFECTOBJMAP::value_type Pair) {return !lstrcmp(Pair.first, pEffectTag); });
	if (iter_Origin == m_OriginEffectOBJMap.end())
	{
		m_OriginEffectOBJMap.insert(ORIGIN_EFFECTOBJMAP::value_type(pEffectTag, pNewEffect));
	}
	else
		return E_FAIL;

	EFFECTOBJLIST EffectList;
	m_RecycleEffectOBJMap.insert(EFFECTOBJMAP::value_type(pEffectTag, EffectList));

	++m_iEffectOriginSize;

	return NOERROR;
}

HRESULT CEffect_Manager::Load_Effect_Resource(const _tchar * pFileFullPath)
{

	HANDLE		hFile_Rsc = CreateFile(pFileFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile_Rsc)
		return E_FAIL;

	_ulong		dwByte = 0;

	CComponent* pComponent = nullptr;

	TCHAR	szComponentTag[MAX_PATH] = L"";
	TCHAR	szFilePath[MAX_PATH] = L"";

	_uint	iListCnt = 0, iMeshCnt = 0, iTexCnt = 0, iLength = 0;

	ReadFile(hFile_Rsc, &iListCnt, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iListCnt; ++i)
	{
		ReadFile(hFile_Rsc, &iMeshCnt, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile_Rsc, &iTexCnt, sizeof(_uint), &dwByte, nullptr);

		for (size_t i = 0; i < iMeshCnt; ++i)
		{
			ReadFile(hFile_Rsc, &iLength, sizeof(_uint), &dwByte, nullptr);
			if (0 == dwByte)
				break;
			ReadFile(hFile_Rsc, szComponentTag, sizeof(TCHAR) * iLength, &dwByte, nullptr);
			ReadFile(hFile_Rsc, szFilePath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);

			Engine::CComponent* pComponent = CComponent_Manager::GetInstance()->Find_Component(0, szComponentTag);
			if (nullptr == pComponent)
			{
				pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, szFilePath, szComponentTag);
				if (nullptr == pComponent)
					return E_FAIL;
				if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(0, CString_Manager::GetInstance()->GetStr(szComponentTag), pComponent)))
					return E_FAIL;
			}
		}

		for (size_t i = 0; i < iTexCnt; ++i)
		{
			ReadFile(hFile_Rsc, &iLength, sizeof(_uint), &dwByte, nullptr);
			if (0 == dwByte)
				break;
			ReadFile(hFile_Rsc, szComponentTag, sizeof(TCHAR) * iLength, &dwByte, nullptr);
			ReadFile(hFile_Rsc, szFilePath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);

			Engine::CComponent* pComponent = CComponent_Manager::GetInstance()->Find_Component(0, szComponentTag);
			if (nullptr == pComponent)
			{
				pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, szFilePath);
				if (nullptr == pComponent)
					return E_FAIL;
				if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(0, CString_Manager::GetInstance()->GetStr(szComponentTag), pComponent)))
					return E_FAIL; // 중복되는 리소스의 경우 그냥 넘어가도록?
			}
		}
	}

	return NOERROR;
}

bool CEffect_Manager::Use_Effect(const _uint & eSceneID, const _tchar * pEffectTag, CGameObject * pTarget, D3DXMATRIX * pBoneMatrix)
{
	if (pEffectTag == nullptr)
		return false;

	if (pTarget == nullptr)
		return false;

	if (pTarget->Get_IsDead())
		return false;

	CEffectOBJ* pEffect = Create_Effect(pEffectTag);

	if (pEffect == nullptr)
		return false;

	CTransform* pTargetTransform = pTarget->Get_Transform();
	
	if(pTargetTransform != nullptr)
		pEffect->Set_Target_Transform(pTargetTransform);

	if (pBoneMatrix != nullptr)
		pEffect->Set_Frame_Pointer(pBoneMatrix);

	if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(eSceneID, L"Effect", pEffect)))
	{
		Delete_Effect(pEffect);
		return false;
	}

	pEffect->AddRef();
	return true;
}

bool CEffect_Manager::Use_Effect(const _uint & eSceneID, const _tchar * pEffectTag, _matrix matWorld)
{
	if (pEffectTag == nullptr)
		return false;

	CEffectOBJ* pEffect = Create_Effect(pEffectTag);

	if (pEffect == nullptr)
		return false;

	pEffect->Set_Matrix(matWorld);

	if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(eSceneID, L"Effect", pEffect)))
	{
		Delete_Effect(pEffect);
		return false;
	}

	pEffect->AddRef();

	return true;
}

bool CEffect_Manager::Use_Effect(const _uint & eSceneID, const _tchar * pEffectTag, _vec3 vPosition)
{
	if (pEffectTag == nullptr)
		return false;

	CEffectOBJ* pEffect = Create_Effect(pEffectTag);

	if (pEffect == nullptr)
		return false;

	_matrix matTrans;

	D3DXMatrixTranslation(&matTrans, vPosition.x, vPosition.y, vPosition.z);

	pEffect->Set_Matrix(matTrans);

	if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(eSceneID, L"Effect", pEffect)))
	{
		Delete_Effect(pEffect);
		return false;
	}

	pEffect->AddRef();

	return true;
}

CEffectOBJ * CEffect_Manager::Create_Effect(const _tchar * pEffectTag)
{
	if (pEffectTag == nullptr)
		return nullptr;

	auto iter_Recycle = find_if(m_RecycleEffectOBJMap.begin(), m_RecycleEffectOBJMap.end(), [&](const EFFECTOBJMAP::value_type Pair) {return !lstrcmp(Pair.first, pEffectTag);  });
	if (iter_Recycle == m_RecycleEffectOBJMap.end())
		return nullptr;

	if (!iter_Recycle->second.empty())
	{
		auto pEffect = *iter_Recycle->second.begin();

		//이펙트 재사용
		pEffect->Reset();
		iter_Recycle->second.pop_front();
		m_EffectOBJList.push_back(pEffect);
		return pEffect;
	}

	//남아있는 비활성화 이펙트가 없다면 클론해서 생성해야함.
	auto iter_Origin = find_if(m_OriginEffectOBJMap.begin(), m_OriginEffectOBJMap.end(), [&](const ORIGIN_EFFECTOBJMAP::value_type Pair) {return !lstrcmp(Pair.first, pEffectTag); });
	if (iter_Origin == m_OriginEffectOBJMap.end())
		return nullptr;

	CEffectOBJ* pEffect = nullptr;
	pEffect = dynamic_cast<CEffectOBJ*>(iter_Origin->second->Clone());
	m_EffectOBJList.push_back(pEffect);

	return pEffect;

}

void CEffect_Manager::Update(const _float & fTimeDelta)
{
	EFFECTOBJLIST::iterator iter_begin = m_EffectOBJList.begin();
	EFFECTOBJLIST::iterator iter_end = m_EffectOBJList.end();
	for (;iter_begin!=iter_end;)
	{
		if ((*iter_begin)->Get_IsDead())
		{
			Delete_Effect((*iter_begin));
			iter_begin = m_EffectOBJList.erase(iter_begin);
		}
		else
			++iter_begin;
	}
}

void CEffect_Manager::LastUpdate(const _float & fTimeDelta)
{

}

bool CEffect_Manager::Delete_Effect(CEffectOBJ * pEffect)
{
	if (pEffect == nullptr)
		return false;

	auto iter_Recycle = find_if(m_RecycleEffectOBJMap.begin(), m_RecycleEffectOBJMap.end(), [&](const EFFECTOBJMAP::value_type Pair) {return !lstrcmp(Pair.first, pEffect->Get_EffectTag());  });
	if (iter_Recycle != m_RecycleEffectOBJMap.end())
		iter_Recycle->second.push_back(pEffect);
	else
		Safe_Release(pEffect);

	return true;
}

#ifndef _FINAL
void CEffect_Manager::Debug_Render()
{

	int iEffectListSize = m_EffectOBJList.size();

	CDebug::GetInstance()->Add_DebugText("CEffect_Manager");
	CDebug::GetInstance()->Add_DebugText("(Cur) EffectListCount:%d", iEffectListSize);
	CDebug::GetInstance()->Add_DebugText(">>>  Donw (VK_DOWN) / UP (VK_UP)  <<<");
	int Count = 0;


	if (GetAsyncKeyState(CDebug::Key[0]) & 0x0001)
		iDebugMenu = 0;
	else if (GetAsyncKeyState(CDebug::Key[1]) & 0x0001)
		iDebugMenu = 1;
	else if (GetAsyncKeyState(CDebug::Key[2]) & 0x0001)
		iDebugMenu = 2;

	if (iDebugMenu == 0) // 현재 게임에서 살아있는 이펙트 시스템
	{
		CDebug::GetInstance()->Add_DebugText("----- Cur Effect List -----");
		for (auto pEffect : m_EffectOBJList)
		{
			
			if (iStartCount > Count)
			{
				++Count;
				continue;
			}
			CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount + 3], pEffect->Get_EffectTag());
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count - iStartCount +3]) & 0x0001)
			{
				iStartCount = 0;
				pEffect->Set_LeafNode();
				CDebug::GetInstance()->Set_CurDebug(std::bind(&CEffectOBJ::Debug_Render, pEffect), pEffect);
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x0001)
			{
				iStartCount += 1;
				if (iStartCount >= iEffectListSize)
					iStartCount = iEffectListSize - 1;
			}
			if (GetAsyncKeyState(VK_UP) & 0x0001)
			{
				iStartCount -= 1;
				if (iStartCount < 0)
					iStartCount = 0;
			}
			++Count;
		}
	}
	else if(iDebugMenu == 1) // 원본 이펙트 시스템
	{ 
		CDebug::GetInstance()->Add_DebugText("----- Origin Effect List -----");
		for (auto pPair : m_OriginEffectOBJMap)
		{
			CEffectOBJ* pEffect = pPair.second;

			if (iStartCount > Count)
			{
				++Count;
				continue;
			}
			CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount + 3], pPair.second);
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count - iStartCount + 3]) & 0x0001)
			{
				iStartCount = 0;
				pEffect->Set_LeafNode();
				CDebug::GetInstance()->Set_CurDebug(std::bind(&CEffectOBJ::Debug_Render, pEffect), pEffect);
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x0001)
			{
				iStartCount += 1;
				if (iStartCount >= iEffectListSize)
					iStartCount = iEffectListSize - 1;
			}
			if (GetAsyncKeyState(VK_UP) & 0x0001)
			{
				iStartCount -= 1;
				if (iStartCount < 0)
					iStartCount = 0;
			}
			++Count;
		}
	}
	else if (iDebugMenu == 2) // 죽은 이펙트 시스템
	{
		CDebug::GetInstance()->Add_DebugText("----- Dead Effect List -----");
		for (auto pLayer : m_RecycleEffectOBJMap)
		{
			for (auto pEffect : pLayer.second)
			{
				if (iStartCount > Count)
				{
					++Count;
					continue;
				}
				CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount + 3], pEffect->Get_EffectTag());
				//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
				if (GetAsyncKeyState(CDebug::Key[Count - iStartCount + 3]) & 0x0001)
				{
					iStartCount = 0;
					pEffect->Set_LeafNode();
					CDebug::GetInstance()->Set_CurDebug(std::bind(&CEffectOBJ::Debug_Render, pEffect), pEffect);
				}
				if (GetAsyncKeyState(VK_DOWN) & 0x0001)
				{
					iStartCount += 1;
					if (iStartCount >= iEffectListSize)
						iStartCount = iEffectListSize - 1;
				}
				if (GetAsyncKeyState(VK_UP) & 0x0001)
				{
					iStartCount -= 1;
					if (iStartCount < 0)
						iStartCount = 0;
				}
				++Count;
			}
		}
	}

}
#endif



HRESULT CEffect_Manager::Release_CurEffectList()
{
	_ulong dwRefCnt = 0;

	for (auto pEffect : m_EffectOBJList)
	{
		dwRefCnt += Safe_Release(pEffect);
	}
	m_EffectOBJList.clear();

	return NOERROR;
}

_ulong CEffect_Manager::Free(void)	//By.Jang
{
	_ulong dwRefCnt = 0;
	//for (size_t i = 0; i < m_iContainerSize; i++)
	//{
	//	for_each(m_pMapComponents[i].begin(), m_pMapComponents[i].end(), CRelease_Pair());
	//	m_pMapComponents[i].clear();
	//}
	//Safe_Delete_Array(m_pMapComponents);
	for (auto pEffect : m_EffectOBJList)
	{
		dwRefCnt += Safe_Release(pEffect);
	}
	m_EffectOBJList.clear();

	for (auto pPair : m_RecycleEffectOBJMap)
	{
		for (auto pEffect : pPair.second)
		{
			dwRefCnt += Safe_Release(pEffect);
		}
		pPair.second.clear();
	}
	m_RecycleEffectOBJMap.clear();

	for (auto pPair : m_OriginEffectOBJMap)
	{
		CEffectOBJ* pEffect = pPair.second;
		dwRefCnt += Safe_Release(pEffect);		
	}
	m_OriginEffectOBJMap.clear();
	

	return dwRefCnt;
}
