#include "..\Headers\Decal_Manager.h"

#include "Shader.h"

USING(Engine)
IMPLEMENT_SINGLETON(CDecal_Manager)

CDecal_Manager::CDecal_Manager()
{
}

void CDecal_Manager::Set_ShaderCom( Engine::CShader* _pShaderCom )
{
	if ( nullptr == _pShaderCom )
		return;

	m_pShaderCom = _pShaderCom;
	m_pShaderCom->AddRef();
}

_uint CDecal_Manager::Get_SSDecalCount( void )
{
	return m_SSDecalInfo.iSSDecalCount;
}

CSSDecal* CDecal_Manager::Get_SSDecal( const _uint & _iIndex )
{
	if ( m_SSDecalInfo.iSSDecalCount <= _iIndex )
		return nullptr;

	return m_SSDecalInfo.ppSSDecal[_iIndex];
}

HRESULT CDecal_Manager::Add_SSDecal( const SSDECALCREATEDATA & _SSDecalCreateData )
{
	CSSDecal* pSSDecal = CSSDecal::Create( m_pGraphicDev, _SSDecalCreateData, m_pShaderCom );

	if ( FAILED( m_SSDecalInfo.Add_SSDecal( pSSDecal ) ) )
		return E_FAIL;

	return NOERROR;

}

void CDecal_Manager::Render_SSDecal( void )
{
	for ( _uint i = 0; i < m_SSDecalInfo.iSSDecalCount; ++i )
	{
		if ( nullptr != m_SSDecalInfo.ppSSDecal[i] )
			m_SSDecalInfo.ppSSDecal[i]->Render_GameObject();
	}
}

#ifdef SSD_DEBUG
void CDecal_Manager::Render_SSDecalDebug( const _uint& _iType )
{
	for ( _uint i = 0; i < m_SSDecalInfo.iSSDecalCount; ++i )
	{
		if ( nullptr != m_SSDecalInfo.ppSSDecal[i] )
			m_SSDecalInfo.ppSSDecal[i]->Render_DebugDecal( _iType );
	}
}
#endif

HRESULT CDecal_Manager::Ready_Decal_Manager( LPDIRECT3DDEVICE9 _pGraphicDev )
{
	if ( nullptr == _pGraphicDev )
		return E_FAIL;

	if ( nullptr == m_pGraphicDev )
	{
		m_pGraphicDev = _pGraphicDev;
		m_pGraphicDev->AddRef();
	}

	return NOERROR;
}

_ulong CDecal_Manager::Free( void )
{
	_ulong dwRefCnt = 0;

	for ( _uint i = 0; i < m_SSDecalInfo.iSSDecalCount; ++i )
	{
		Engine::Safe_Release( m_SSDecalInfo.ppSSDecal[i] );
	}
	Engine::Safe_Delete_Array( m_SSDecalInfo.ppSSDecal );
	m_SSDecalInfo.iSSDecalCount = 0;

	dwRefCnt = Engine::Safe_Release( m_pGraphicDev );
	dwRefCnt = Engine::Safe_Release( m_pShaderCom );

	return 0;
}
