#include "Texture.h"

USING(Engine)

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CTexture", std::bind(&CTexture::Debug_Render, this));
#endif

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_vecTextures(rhs.m_vecTextures)
{
	for (auto& pTexture : m_vecTextures)
		pTexture->AddRef();
#ifndef _FINAL
	Tag(L"CTexture", std::bind(&CTexture::Debug_Render, this));
#endif

}

HRESULT CTexture::Ready_Texture(TEXTURETYPE eType, const _tchar * pFilePath, const _uint & iTexCnt)
{
	m_vecTextures.reserve(iTexCnt);

	for (size_t i = 0; i < iTexCnt; ++i)
	{
		_tchar			szFilePath[128] = L"";

		wsprintf(szFilePath, pFilePath, i);

		if(TYPE_CUBE == eType)
		{
			LPDIRECT3DCUBETEXTURE9		pTexture = nullptr;
			D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFilePath, &pTexture);
			m_vecTextures.push_back(pTexture);

		}
		else
		{
			LPDIRECT3DTEXTURE9		pTexture = nullptr;
			D3DXCreateTextureFromFile(m_pGraphicDev, szFilePath, &pTexture);
			m_vecTextures.push_back(pTexture);
		}		
	}	

	return NOERROR;
}

HRESULT CTexture::SetUp_OnGraphicDev(const _ulong & dwStage, const _uint & iCnt)
{
	if (iCnt >= m_vecTextures.size())
		return E_FAIL;

	m_pGraphicDev->SetTexture(dwStage, m_vecTextures[iCnt]);

	return NOERROR;
}

HRESULT CTexture::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName, const _uint & iCnt)
{
	if (iCnt >= m_vecTextures.size())
		return E_FAIL;

	pEffect->SetTexture(pConstantName, m_vecTextures[iCnt]);

	return NOERROR;
}



CComponent * CTexture::Clone(void)
{
	return new CTexture(*this);
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTURETYPE eType, const _tchar * pFilePath, const _uint & iTexCnt)
{
	CTexture *	pInstance = new CTexture(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(eType, pFilePath, iTexCnt)))
	{
		MessageBox(0, L"CTriangle_Color Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CTexture::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTexture");

}
#endif
_ulong CTexture::Free(void)
{
	_ulong dwRefCnt = 0;

	for (auto& pTexture : m_vecTextures)	
		Safe_Release(pTexture);	
	m_vecTextures.clear();

	dwRefCnt = CComponent::Free();

	return dwRefCnt;	
}
