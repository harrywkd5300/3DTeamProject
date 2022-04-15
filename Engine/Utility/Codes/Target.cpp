#include "..\Headers\Target.h"
#include "Debug_Manager.h"

USING(Engine)

CTarget::CTarget(LPDIRECT3DDEVICE9 pGrapihcDev)
	: m_pGraphicDev(pGrapihcDev)
#ifdef RENDERTARGETNAME
	, m_pDebug(CDebug::GetInstance())
#endif
{
	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CTarget", std::bind(&CTarget::Debug_Render, this));
#endif

}

HRESULT CTarget::Get_SurfaceLevel( const _uint & iLevel, LPDIRECT3DSURFACE9 * pOut )
{
	if ( FAILED( m_pTargetTexture->GetSurfaceLevel( iLevel, pOut ) ) )
		return E_FAIL;

	return NOERROR;
}

LPDIRECT3DTEXTURE9 CTarget::Get_Texture( void )
{
	return m_pTargetTexture;
}

HRESULT CTarget::Ready_Target(const _uint& iTargetCX, const _uint& iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color)
{
	m_ClearColor = Color;

	if(FAILED(D3DXCreateTexture(m_pGraphicDev, iTargetCX, iTargetCY, 1, D3DUSAGE_RENDERTARGET, eFormat, D3DPOOL_DEFAULT, &m_pTargetTexture)))
		return E_FAIL;

	// 렌더 타겟에 대한 정보를 텍스쳐로부터 얻어온다.
	m_pTargetTexture->GetSurfaceLevel(0, &m_pRenderTarget); // m_pTarget_Texture라는 녀석의 참조개수가 증가한다.

	Safe_Release(m_pTargetTexture);

#ifdef KMH_MODIFY // By.KMH
	Clear_Target();
#endif
	
	return NOERROR;
}

void CTarget::Clear_Target(void)
{
	m_pGraphicDev->GetRenderTarget(0, &m_pOldTarget);

	m_pGraphicDev->SetRenderTarget(0, m_pRenderTarget);

	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	m_pGraphicDev->SetRenderTarget(0, m_pOldTarget);

	Safe_Release(m_pOldTarget);
}

void CTarget::Save_Target( const _tchar * pName )
{
	_tchar szFileName[256] = L"";
	swprintf_s( szFileName, L"../Bin/%s.jpg", pName );
	D3DXSaveTextureToFile( szFileName, D3DXIFF_JPG, m_pTargetTexture, nullptr );
}

HRESULT CTarget::Ready_DebugBuffer(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY)
{
	if(FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXTEX_VIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXTEX_VIEWPORT*	pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec4(fX, fY, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4(fX + fSizeX, fY, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(fX + fSizeX, fY + fSizeY, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(fX, fY + fSizeY, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	if(FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

#ifdef RENDERTARGETNAME
	m_fPosX = fX;
	m_fPosY = fY;
#endif
	
	return NOERROR;
}

#ifdef RENDERTARGETNAME
void CTarget::Render_DebugBuffer( const _tchar* _pName )
{
	m_pGraphicDev->SetTexture(0, m_pTargetTexture);

	if ( m_szName[0] == '\0' )
	{
		_uint jLength = lstrlen( _pName );
		_bool bFind = false;
		_tchar szName[256] = L"";
		for ( size_t j = 0; j < jLength; ++j )
		{
			if ( _pName[j] == L'_' )
			{
				memcpy( szName, _pName + j + 1, sizeof( _tchar ) * (jLength - j - 1) );
				m_szName[jLength - j - 1] = '\0';
				bFind = true;
				break;
			}
		}
		if ( false == bFind )
			lstrcpy( szName, _pName );

		int len = WideCharToMultiByte( CP_ACP, 0, szName, -1, NULL, 0, NULL, NULL );
		WideCharToMultiByte( CP_ACP, 0, szName, -1, m_szName, len, NULL, NULL );
	}

	m_pDebug->Add_DebugText( m_fPosX, m_fPosY, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), m_szName );

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX_VIEWPORT));
	m_pGraphicDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}
#else
void CTarget::Render_DebugBuffer(void)
{
	m_pGraphicDev->SetTexture(0, m_pTargetTexture);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX_VIEWPORT));
	m_pGraphicDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}
#endif

HRESULT CTarget::SetUp_OnGraphicDev(const _uint & iIndex)
{
	m_pGraphicDev->GetRenderTarget(iIndex, &m_pOldTarget);

	// 특정 인덱스에 렌더타겟을 셋팅합니다.
	m_pGraphicDev->SetRenderTarget(iIndex, m_pRenderTarget);

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	pEffect->SetTexture(pConstantName, m_pTargetTexture);

	return NOERROR;
}

HRESULT CTarget::Release_OnGraphicDev(const _uint & iIndex)
{
	m_pGraphicDev->SetRenderTarget(iIndex, m_pOldTarget);

	Safe_Release(m_pOldTarget);

	return NOERROR;
}

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGrapihcDev, const _uint& iTargetCX, const _uint& iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color)
{
	CTarget *		pInstance = new CTarget(pGrapihcDev);

	if(FAILED(pInstance->Ready_Target(iTargetCX, iTargetCY, eFormat, Color)))
	{
		MSG_BOX("CTarget Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CTarget::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTarget");

}
#endif
_ulong CTarget::Free(void)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pTargetTexture);

	Safe_Release(m_pGraphicDev);

	return _ulong();
}
