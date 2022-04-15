#include "Texture.h"
#include "..\Headers\MultiTexture.h"
#include "atlstr.h"

USING(Engine)

CMultiTexture::CMultiTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
	for (int i = 0; i < MULTITEXTURETYPE::TEX_TYPE_END; ++i)
	{
		m_pTextureArr[i] = nullptr;
	}
#ifndef _FINAL
	Tag(L"CMultiTexture", std::bind(&CMultiTexture::Debug_Render, this));
#endif

}

CMultiTexture::CMultiTexture(const CMultiTexture & rhs)
	: CComponent(rhs)
{
	for (int i = 0; i < MULTITEXTURETYPE::TEX_TYPE_END; ++i)
	{
		if (nullptr != rhs.m_pTextureArr[i])
		{
			rhs.m_pTextureArr[i]->AddRef();
			m_pTextureArr[i] = rhs.m_pTextureArr[i];
		}
		else
		{
			m_pTextureArr[i] = nullptr;
		}
	}
#ifndef _FINAL
	Tag(L"CMultiTexture", std::bind(&CMultiTexture::Debug_Render, this));
#endif

}


HRESULT CMultiTexture::Ready_Texture(const _tchar * pFilePath, const _tchar * pFileName)
{
	_tchar			szFullPath[128] = L"";
	_tchar			szFileName[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcpy(szFileName, pFileName);

	lstrcat(szFullPath, szFileName);

	HANDLE		hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return false;

	_ulong			dwByte = 1;
	_ulong			dwNumCell = 0;
	_vec3			vPoint[3];
	_ulong			iIndex = 0;

	char			szTextureFileName[256] = "";
	_tchar			tzTextureFileName[128] = L"";
	int iCount = 0;
	while (dwByte != 0)
	{
		ReadFile(hFile, (char*)(szTextureFileName + iCount), sizeof(char), &dwByte, nullptr);
		++iCount;
	}

	CString TextureFileName = szTextureFileName;
	int iTexNameIndex = 0;
	int CheckFindIndex = 0;
	// ------------- Diffuse Texture ------------- 
	CheckFindIndex = TextureFileName.Find(L"Diffuse=", iTexNameIndex);
	if (CheckFindIndex == -1)
	{
		m_pTextureArr[TEXTUREDATA::TEX_DIFFUSE] = nullptr;
	}
	else
	{
		iTexNameIndex = CheckFindIndex + 8;
		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
		lstrcat(tzTextureFileName, L".tga");
		lstrcpy(szFullPath, pFilePath);
		lstrcat(szFullPath, tzTextureFileName);
		D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_pTextureArr[TEXTUREDATA::TEX_DIFFUSE]);
	}
	
	// ------------- Normal Texture ------------- 
	CheckFindIndex = TextureFileName.Find(L"Normal=", iTexNameIndex);
	if (CheckFindIndex == -1)
	{
		m_pTextureArr[TEXTUREDATA::TEX_NORMAL] = nullptr;
	}
	else
	{
		iTexNameIndex = CheckFindIndex + 7;
		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
		lstrcat(tzTextureFileName, L".tga");
		lstrcpy(szFullPath, pFilePath);
		lstrcat(szFullPath, tzTextureFileName);
		D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_pTextureArr[TEXTUREDATA::TEX_NORMAL]);
	}
	// ------------- Specualr Texture ------------- 
	CheckFindIndex = TextureFileName.Find(L"Specular=", iTexNameIndex);
	if (CheckFindIndex == -1)
	{
		m_pTextureArr[TEXTUREDATA::TEX_SPECULAR] = nullptr;
	}
	else
	{
		iTexNameIndex = CheckFindIndex + 9;
		lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
		lstrcat(tzTextureFileName, L".tga");
		lstrcpy(szFullPath, pFilePath);
		lstrcat(szFullPath, tzTextureFileName);
		D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_pTextureArr[TEXTUREDATA::TEX_SPECULAR]);
	}

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CMultiTexture::SetUp_OnShader(LPD3DXEFFECT pEffect, MULTITEXTURETYPE eType, const char * pConstantName)
{
	if (m_pTextureArr[eType] == nullptr)
		return E_FAIL;

	pEffect->SetTexture(pConstantName, m_pTextureArr[eType]);

	return NOERROR;
}



CComponent * CMultiTexture::Clone(void)
{
	return new CMultiTexture(*this);
}

CMultiTexture * CMultiTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName)
{
	CMultiTexture *	pInstance = new CMultiTexture(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(pFilePath, pFileName)))
	{
		MessageBox(0, L"CMultiTexture Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CMultiTexture::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMultiTexture");

}
#endif

_ulong CMultiTexture::Free(void)
{
	_ulong dwRefCnt = 0;

	for (int i = 0; i < MULTITEXTURETYPE::TEX_TYPE_END; ++i)
	{
		Safe_Release(m_pTextureArr[i]);
	}

	dwRefCnt = CComponent::Free();

	return dwRefCnt;	
}
