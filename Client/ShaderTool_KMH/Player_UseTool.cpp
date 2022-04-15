#include "stdafx.h"
#include "Player_UseTool.h"

#include "Component_Manager.h"

CPlayer_UseTool::CPlayer_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CGameObject( _pGraphicDev )
{
}

HRESULT CPlayer_UseTool::Ready_GameObject( void )
{
	Engine::CComponent* pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CTransform" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CMesh_Dynamic_Donatello" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CShader_Mesh" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	m_pEffect = m_pShaderCom->Get_EffectHandle();
	if ( nullptr == m_pEffect )
		return E_FAIL;

	m_pEffect->AddRef();

	return NOERROR;
}

_int CPlayer_UseTool::Update_GameObject( const _float & _fTimeDelta )
{
	return _int();
}

_int CPlayer_UseTool::LastUpdate_GameObject( const _float & _fTimeDelta )
{
	return _int();
}

void CPlayer_UseTool::Render_GameObject( void )
{
}

_ulong CPlayer_UseTool::Free( void )
{
	_ulong dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release( m_pShaderCom );
	dwRefCnt = Engine::Safe_Release( m_pTransformCom );
	dwRefCnt = Engine::Safe_Release( m_pRendererCom );
	dwRefCnt = Engine::Safe_Release( m_pMeshCom );
	dwRefCnt = Engine::Safe_Release( m_pEffect );

	return Engine::CGameObject::Free();
}
