#include "stdafx.h"
#include "Object_ToolUse.h"

#include "Component_Manager.h"

CObject_ToolUse::CObject_ToolUse( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CGameObject( _pGraphicDev )
{
}

HRESULT CObject_ToolUse::Ready_Object( const OBJECTDATA & _ObjectData )
{
	if ( FAILED( Ready_Component( _ObjectData ) ) )
		EFAIL_MSGBOX( L"Ready_Component" );

	if ( FAILED( Ready_DefaultSetting( _ObjectData ) ) )
		EFAIL_MSGBOX( L"Ready_DefaultSetting" );

	return NOERROR;
}

_int CObject_ToolUse::Update_GameObject( const _float & fTimeDelta )
{
	if ( nullptr == m_pRendererCom )
		return -1;
	if ( FAILED( m_pRendererCom->Add_RenderGroup( Engine::CRenderer::RENDER_NONALPHA, this ) ) )
		return E_FAIL;

	m_fTimeDelta = fTimeDelta;

	return Engine::CGameObject::Update_GameObject( fTimeDelta );
}

void CObject_ToolUse::Render_GameObject( void )
{
	if ( FAILED( SetUp_OnConstantTable( m_pEffect ) ) )
		return;

	if ( m_bisStaticMesh )
	{
		m_pStaticMeshCom->Render_Mesh();
	}
	else
	{
		m_pDynamicMeshCom->Play_Animation( m_fTimeDelta );
		m_pDynamicMeshCom->Render_Mesh();
	}
}

CObject_ToolUse * CObject_ToolUse::Create( LPDIRECT3DDEVICE9 _pGraphicDev, const OBJECTDATA & _ObjectData )
{
	CObject_ToolUse* pInstance = new CObject_ToolUse( _pGraphicDev );

	if ( FAILED( pInstance->Ready_Object( _ObjectData ) ) )
	{
		CREATE_FAIL( pInstance, "CObject_ToolUse" );
	}

	return pInstance;
}

HRESULT CObject_ToolUse::Ready_Component( const OBJECTDATA & _ObjectData )
{
	Engine::CComponent* pComponent = nullptr;

	// For.Renderer..
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" );
	if (nullptr == pComponent)
		return E_FAIL;

	// For.Transform..
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CTransform" );
	if (nullptr == pComponent)
		return E_FAIL;

	// For.Shader..
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CShader_Mesh" );
	if (nullptr == pComponent)
		return E_FAIL;

	if ( _ObjectData.bStaticMesh == true )
	{

		// For Mesh Static..
		pComponent = m_pStaticMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, _ObjectData.pMeshTag );
		if (nullptr == pComponent)
			return E_FAIL;

	}
	else
	{

		// For.Mesh Dynamic..
		pComponent = m_pDynamicMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, _ObjectData.pMeshTag );
		if (nullptr == pComponent)
			return E_FAIL;

	}

	return NOERROR;
}

HRESULT CObject_ToolUse::Ready_DefaultSetting( const OBJECTDATA & _ObjectData )
{
	if ( nullptr == m_pTransformCom ||
		 nullptr == m_pShaderCom )
		EFAIL_MSGBOX( L"Ready_DefaultSetting" );

	// --------------- Get EffectHandle.. --------------- //
	m_pEffect = m_pShaderCom->Get_EffectHandle();

	// --------------- Transform Setting.. --------------- //
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_SCALE, &_ObjectData.vScale );

	return NOERROR;
}

HRESULT CObject_ToolUse::SetUp_OnConstantTable( LPD3DXEFFECT _pEffect )
{
	_pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader( _pEffect, "g_matWorld" );

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform( D3DTS_VIEW, &matView );
	m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &matProj );

	_pEffect->SetMatrix( "g_matView", &matView );
	_pEffect->SetMatrix( "g_matProj", &matProj );

	Safe_Release( _pEffect );

	return NOERROR;
}

_ulong CObject_ToolUse::Free( void )
{
	_ulong dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release(m_pEffect);
	dwRefCnt = Engine::Safe_Release(m_pShaderCom);
	dwRefCnt = Engine::Safe_Release(m_pRendererCom);
	dwRefCnt = Engine::Safe_Release(m_pTransformCom);
	if ( m_bisStaticMesh )
		dwRefCnt = Engine::Safe_Release( m_pStaticMeshCom );
	else
		dwRefCnt = Engine::Safe_Release( m_pDynamicMeshCom );

	return Engine::CGameObject::Free();
}
