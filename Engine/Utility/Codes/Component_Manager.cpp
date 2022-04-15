#include "..\Headers\Component_Manager.h"
#include "Management.h"
#include "Camera_Manager.h"
#include "atlstr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
	: m_pMapComponents(nullptr)
{
#ifndef _FINAL
	Tag(L"CComponent_Manager", std::bind(&CComponent_Manager::Debug_Render, this));
#endif

}

HRESULT CComponent_Manager::Ready_Component(const _uint& iArrayIdx, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == m_pMapComponents
		|| iArrayIdx >= m_iContainerSize)
		return E_FAIL;
	

	CComponent* pComponent_Find = Find_Component(iArrayIdx, pComponentTag);
	if (nullptr != pComponent_Find)
		return E_FAIL;

	m_pMapComponents[iArrayIdx].insert(MAPCOMPONENTS::value_type(pComponentTag, pComponent));
	++m_iComponentTotalSize;

	return NOERROR;
}

CComponent* CComponent_Manager::Clone_Component(const _uint & iArrayIdx, const _tchar * pComponentTag)
{
	if (iArrayIdx >= m_iContainerSize)
		return nullptr;

	CComponent* pComponent = Find_Component(iArrayIdx, pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone();
}
//
//HRESULT CComponent_Manager::Load_Texture_ByMatFile(const _uint & iArrayIdx, const _tchar * pComponentTag, CComponent * pComponent, LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName, TEXTUREDATA * pOut)
//{
//	if (nullptr == pOut)
//		return E_FAIL;
//
//	for (int i = 0; i < TEXTUREDATA::TEX_END; ++i)
//	{
//		pOut->pTexture[i] = nullptr;
//	}
//
//	_tchar			szFullPath[128] = L"";
//	_tchar			szFileName[128] = L"";
//
//	lstrcpy(szFullPath, pFilePath);
//	lstrcpy(szFileName, pFileName);
//
//	lstrcat(szFullPath, szFileName);
//
//	HANDLE		hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//	if (0 == hFile)
//		return false;
//
//	_ulong			dwByte = 1;
//	_ulong			dwNumCell = 0;
//	_vec3			vPoint[3];
//	_ulong			iIndex = 0;
//
//	char			szTextureFileName[256] = "";
//	_tchar			tzTextureFileName[128] = L"";
//	int iCount = 0;
//	while (dwByte != 0)
//	{
//		ReadFile(hFile, (char*)(szTextureFileName + iCount), sizeof(char), &dwByte, nullptr);
//		++iCount;
//	}
//
//	CString TextureFileName = szTextureFileName;
//	int iTexNameIndex = 0;
//	int CheckFindIndex = 0;
//	// ------------- Diffuse Texture ------------- 
//	CheckFindIndex = TextureFileName.Find(L"Diffuse=", iTexNameIndex);
//	if (CheckFindIndex == -1)
//		return E_FAIL;
//	else
//	{
//		iTexNameIndex = CheckFindIndex + 8;
//		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
//		lstrcat(tzTextureFileName, L".tga");
//		lstrcpy(szFullPath, pFilePath);
//		lstrcat(szFullPath, tzTextureFileName);
//		D3DXCreateTextureFromFile(pGraphicDev, szFullPath, &pOut->pTexture[TEXTUREDATA::TEX_DIFFUSE]);
//	}
//
//	// ------------- Normal Texture ------------- 
//	CheckFindIndex = TextureFileName.Find(L"Normal=", iTexNameIndex);
//	if (CheckFindIndex == -1)
//	{
//
//	}
//	else
//	{
//		iTexNameIndex = CheckFindIndex + 7;
//		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
//		lstrcat(tzTextureFileName, L".tga");
//		lstrcpy(szFullPath, pFilePath);
//		lstrcat(szFullPath, tzTextureFileName);
//		D3DXCreateTextureFromFile(pGraphicDev, szFullPath, &pOut->pTexture[TEXTUREDATA::TEX_NORMAL]);
//	}
//	// ------------- Specualr Texture ------------- 
//	CheckFindIndex = TextureFileName.Find(L"Specular=", iTexNameIndex);
//	if (CheckFindIndex == -1)
//	{
//
//	}
//	else
//	{
//		iTexNameIndex = CheckFindIndex + 9;
//		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
//		lstrcat(tzTextureFileName, L".tga");
//		lstrcpy(szFullPath, pFilePath);
//		lstrcat(szFullPath, tzTextureFileName);
//		D3DXCreateTextureFromFile(pGraphicDev, szFullPath, &pOut->pTexture[TEXTUREDATA::TEX_SPECULAR]);
//	}
//
//	CloseHandle(hFile);
//
//	return NOERROR;
//}

HRESULT CComponent_Manager::Reserve_ComponentContainer(const _uint & iMaxSize, LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (nullptr != m_pMapComponents)
	{
		MSG_BOX("Reserve_ResourceContainer Failed");
		return E_FAIL;
	}

	m_pMapComponents = new MAPCOMPONENTS[iMaxSize];

	m_iContainerSize = iMaxSize;

	Engine::CComponent*		pComponent = nullptr;

	// For.Component_Shader_Effect
	pComponent = CShader::Create(pGraphicDev, L"../Bin/ShaderFiles/Shader_Effect.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CShader_Effect", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Mesh
	pComponent = CShader::Create(pGraphicDev, L"../Bin/ShaderFiles/Shader_Mesh.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CShader_Mesh", pComponent)))
		return E_FAIL;

	//--For.Transform
	pComponent = Engine::CTransform::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CTransform", pComponent)))
		return E_FAIL;

	//--For.Picking
	pComponent = Engine::CPicking::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CPicking", pComponent)))
		return E_FAIL;

	//--For.Frustum
	pComponent = CFrustum::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if ( FAILED( Ready_Component( 0, L"CFrustum", pComponent ) ) )
		return E_FAIL;

	//--For.Renderer
	pComponent = Engine::CRenderer::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CRenderer", pComponent)))
		return E_FAIL;

	CManagement::GetInstance()->SetUp_Renderer((CRenderer*)pComponent);
	CCamera_Manager::GetInstance()->SetUp_Renderer((CRenderer*)pComponent);


	//--For.Buffers
	pComponent = Engine::CCube_Texture::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CCube_Texture", pComponent)))
		return E_FAIL;

	//--For.Buffers
	pComponent = Engine::CRect_Color::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CRect_Color", pComponent)))
		return E_FAIL;

	//--For.Buffers
	pComponent = Engine::CRect_Texture::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CRect_Texture", pComponent)))
		return E_FAIL;

	//--For.Buffers
	pComponent = Engine::CRectUI_Texture::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CRectUI_Texture", pComponent)))
		return E_FAIL;

	//--For.Buffers
	pComponent = Engine::CTriangle_Color::Create(pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Ready_Component(0, L"CTriangle_Color", pComponent)))
		return E_FAIL;




	return NOERROR;
}

HRESULT CComponent_Manager::Release_Component(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (auto& Pair : m_pMapComponents[iSceneIdx])
	{
		if (dwRefCnt = Safe_Release(Pair.second))
		{
			MSG_BOX("Scene`s Component Release Failed");
			return E_FAIL;
		}
		--m_iComponentTotalSize;
	}
	m_pMapComponents[iSceneIdx].clear();


	return NOERROR;
}

HRESULT CComponent_Manager::Delete_Component(const _uint & iArrayIdx, const _tchar * pComponentTag)
{
	auto iter = find_if(m_pMapComponents[iArrayIdx].begin(), m_pMapComponents[iArrayIdx].end(), CFind_Tag(pComponentTag));

	if (iter == m_pMapComponents[iArrayIdx].end())
	{
		return E_FAIL;
	}
	else
	{
		Safe_Release((*iter).second);
		m_pMapComponents[iArrayIdx].erase(iter);
		--m_iComponentTotalSize;
		return NOERROR;
	}
}

#ifndef _FINAL
void CComponent_Manager::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CComponent_Manager");
	CDebug::GetInstance()->Add_DebugText("m_iComponentTotalSize:%d", m_iComponentTotalSize);
	CDebug::GetInstance()->Add_DebugText(">>>  Donw (VK_DOWN) / UP (VK_UP)  <<<");
	int Count = 0;
	for (int i = 0; i<m_iContainerSize; ++i)
	{
		for (auto pLayer : m_pMapComponents[i])
		{
			if (iStartCount > Count)
			{
				++Count;
				continue;
			}
			CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count- iStartCount], pLayer.first);
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count - iStartCount]) & 0x0001)
			{
				iStartCount = 0;
				pLayer.second->Set_LeafNode();
				CDebug::GetInstance()->Set_CurDebug(std::bind(&CComponent::Debug_Render, pLayer.second), pLayer.second);
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x0001)
			{
				iStartCount += 1;
				if (iStartCount >= m_iComponentTotalSize)
					iStartCount = m_iComponentTotalSize -1;
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
#endif

CComponent * CComponent_Manager::Find_Component(const _uint& iArrayIdx, const _tchar * pComponentTag)
{
	if (nullptr == m_pMapComponents
		|| iArrayIdx >= m_iContainerSize)
		return nullptr;

	auto iter = find_if(m_pMapComponents[iArrayIdx].begin(), m_pMapComponents[iArrayIdx].end(), CFind_Tag(pComponentTag));

	if(iter == m_pMapComponents[iArrayIdx].end())		
		return nullptr;

	return iter->second;
}

_ulong CComponent_Manager::Free(void)	//By.Jang
{
	for (size_t i = 0; i < m_iContainerSize; i++)
	{
		for_each(m_pMapComponents[i].begin(), m_pMapComponents[i].end(), CRelease_Pair());
		m_pMapComponents[i].clear();
	}
	Safe_Delete_Array(m_pMapComponents);
	return _ulong();
}
