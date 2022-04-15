#include "..\Headers\SSDecal.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

USING(Engine)

CSSDecal::CSSDecal( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CGameObject( _pGraphicDev )
	, m_pCam_Manager( CCamera_Manager::GetInstance() )
{
	m_pCam_Manager->AddRef();
}

HRESULT CSSDecal::Ready_SSDecal( const SSDECALCREATEDATA & _SSDecalCreateData, Engine::CShader* _pShaderCom )
{
	if ( FAILED( Ready_Component( _SSDecalCreateData, _pShaderCom ) ) )
		return E_FAIL;

	if ( FAILED( Ready_DefaultSetting( _SSDecalCreateData ) ) )
		return E_FAIL;

	return NOERROR;
}

_int CSSDecal::Update_GameObject( const _float & _fTimeDelta )
{
	return Engine::CGameObject::Update_GameObject( _fTimeDelta );
}

_int CSSDecal::LastUpdate_GameObject( const _float & _fTimeDelta )
{
	return Engine::CGameObject::LastUpdate_GameObject( _fTimeDelta );
}

void CSSDecal::Render_GameObject( void )
{
	if ( nullptr == m_pEffect ||
		 nullptr == m_pBufferCom )
		return;

	if ( FAILED( SetUp_OnConstantTable( m_pEffect ) ) )
		return;

	m_pEffect->BeginPass( 0 );

	m_pBufferCom->Render_Buffer();

	m_pEffect->EndPass();
}

#ifdef SSD_DEBUG
void CSSDecal::Render_DebugDecal( const _uint& _iType )
{
	m_pGraphicDev->SetRenderState( D3DRS_ZENABLE, false );
	m_pGraphicDev->SetRenderState( D3DRS_ZWRITEENABLE, false );
	if(_iType == 1 )
		m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	m_pGraphicDev->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_pBufferCom->Render_Buffer();

	if(_iType == 1 )
		m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pGraphicDev->SetRenderState( D3DRS_ZENABLE, true );
	m_pGraphicDev->SetRenderState( D3DRS_ZWRITEENABLE, true );
}
#endif

HRESULT CSSDecal::Ready_Component( const SSDECALCREATEDATA & _SSDecalCreateData, Engine::CShader* _pShaderCom )
{
	Engine::CComponent* pComponent = nullptr;

	// For.Transform
	if ( nullptr == m_pTransformCom )
	{
		pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(CComponent_Manager::GetInstance()->Clone_Component( 0, L"CTransform" ));
		if ( nullptr == pComponent )
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert( MAPCOMPONENT::value_type( L"Com_Transform", pComponent ) );
		m_pTransformCom->AddRef();
	}

	// For.Renderer
	if ( nullptr == m_pRendererCom )
	{
		pComponent = m_pRendererCom = dynamic_cast<Engine::CRenderer*>(CComponent_Manager::GetInstance()->Clone_Component( 0, L"CRenderer" ));
		if ( nullptr == pComponent )
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert( MAPCOMPONENT::value_type( L"Com_Renderer", pComponent ) );
		m_pRendererCom->AddRef();
	}

	// For.Texture
	{
		if ( nullptr != m_pTextureCom )
			Engine::Safe_Release( m_pTextureCom );

		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(CComponent_Manager::GetInstance()->Clone_Component( _SSDecalCreateData.iImageSceneIndex, _SSDecalCreateData.szImageName ));
		if ( nullptr == pComponent )
			return E_FAIL;

		auto& iter = find_if( m_mapComponent[Engine::CComponent::TYPE_STATIC].begin(), m_mapComponent[Engine::CComponent::TYPE_STATIC].end(), CFind_Tag( L"Com_Texture" ) );
		if ( iter != m_mapComponent[Engine::CComponent::TYPE_STATIC].end() )
		{
			Engine::Safe_Release( iter->second );
			m_mapComponent[Engine::CComponent::TYPE_STATIC].erase( iter );
		}

		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert( MAPCOMPONENT::value_type( L"Com_Texture", pComponent ) );
		m_pTextureCom->AddRef();
	}

	// For.Buffer
	if ( nullptr == m_pBufferCom )
	{
		pComponent = m_pBufferCom = dynamic_cast<Engine::CCube_Texture*>(CComponent_Manager::GetInstance()->Clone_Component( 0, L"CCube_Texture" ));
		if ( nullptr == pComponent )
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert( MAPCOMPONENT::value_type( L"Com_Buffer", pComponent ) );
		m_pBufferCom->AddRef();
	}

	if ( nullptr == m_pShaderCom )
	{
		if ( nullptr == _pShaderCom )
			return E_FAIL;

		m_pShaderCom = _pShaderCom;
		m_pShaderCom->AddRef();
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert( MAPCOMPONENT::value_type( L"Com_Shader", m_pShaderCom ) );
		m_pShaderCom->AddRef();
	}

	return NOERROR;
}

HRESULT CSSDecal::Ready_DefaultSetting( const SSDECALCREATEDATA & _SSDecalCreateData )
{
	if ( nullptr == m_pTransformCom ||
		 nullptr == m_pShaderCom )
		return E_FAIL;

	_vec3 vRotationRadian( 0.f, 0.f, 0.f );

	vRotationRadian.x = D3DXToRadian( _SSDecalCreateData.vRotationDegree.x );
	vRotationRadian.y = D3DXToRadian( _SSDecalCreateData.vRotationDegree.y );
	vRotationRadian.z = D3DXToRadian( _SSDecalCreateData.vRotationDegree.z );

	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &_SSDecalCreateData.vPosition );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_ANGLE, &vRotationRadian );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_SCALE, &_SSDecalCreateData.vScale );

	if ( nullptr == m_pEffect )
	{
		m_pEffect = m_pShaderCom->Get_EffectHandle();

		if ( nullptr == m_pEffect )
			return E_FAIL;

		m_pEffect->AddRef();
	}

	m_matWorld = m_pTransformCom->Get_WorldMatrix_FORCED();
	D3DXMatrixInverse( &m_matInvWorld, nullptr, &m_matWorld );

	return NOERROR;
}

HRESULT CSSDecal::SetUp_OnConstantTable( LPD3DXEFFECT _pEffect )
{
	if ( nullptr == _pEffect )
		return E_FAIL;

	_pEffect->AddRef();

	_matrix matInvView = m_pCam_Manager->Get_ViewMatrix();
	_matrix matInvWorldView = m_matWorld * matInvView;
	D3DXMatrixInverse( &matInvWorldView, nullptr, &matInvWorldView );

	m_pTextureCom->SetUp_OnShader( _pEffect, "g_BaseTexture" );

	_pEffect->SetMatrix( "g_matWorld", &m_matWorld );
	_pEffect->SetMatrix( "g_matInvWorldView", &matInvWorldView );

	_pEffect->CommitChanges();

	Engine::Safe_Release( _pEffect );

	return NOERROR;
}

CSSDecal * CSSDecal::Create( LPDIRECT3DDEVICE9 _pGraphicDev, const SSDECALCREATEDATA & _SSDecalCreateData, Engine::CShader* _pShaderCom )
{
	CSSDecal* pInstance = new CSSDecal( _pGraphicDev );

	if ( FAILED( pInstance->Ready_SSDecal( _SSDecalCreateData, _pShaderCom ) ) )
	{
		CREATE_FAIL( pInstance, "CSSDecal" );
	}

	return pInstance;
}

_ulong CSSDecal::Free( void )
{
	_ulong dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release( m_pCam_Manager );

	dwRefCnt = Engine::Safe_Release( m_pBufferCom );
	dwRefCnt = Engine::Safe_Release( m_pTextureCom );
	dwRefCnt = Engine::Safe_Release( m_pRendererCom );
	dwRefCnt = Engine::Safe_Release( m_pEffect );
	dwRefCnt = Engine::Safe_Release( m_pShaderCom );

	return Engine::CGameObject::Free();
}
