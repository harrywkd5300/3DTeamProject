#include "stdafx.h"
#include "Scene_Tool.h"

#include "Camera_UseTool.h"
#include "Terrain_ToolUse.h"
#include "Object_ToolUse.h"

#include "Light_Manager.h"

CScene_Tool::CScene_Tool( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CScene( _pGraphicDev )
{
}

HRESULT CScene_Tool::Ready_Scene( void )
{

	if ( FAILED( Ready_Layer_Camera( L"Layer_Camera", &_vec3( 95.f, 15.f, 95.f ), &_vec3( 100.f, 5.f, 100.f ), &_vec3( 0.f, 1.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( Ready_Layer_Terrain( L"Layer_Terrain" ) ) )
		return E_FAIL;

	//if ( FAILED( Ready_Layer_Object( L"Layer_Object" ) ) )
	//	return E_FAIL;

	return NOERROR;

}

_int CScene_Tool::Update_Scene( const _float & _fTimeDelta )
{

	return CScene::Update_Scene( _fTimeDelta );

}

_int CScene_Tool::LastUpdate_Scene( const _float & _fTimeDelta )
{

	return CScene::LastUpdate_Scene( _fTimeDelta );

}

void CScene_Tool::Render_Scene( void )
{

	CScene::Render_Scene();

}

HRESULT CScene_Tool::Ready_Layer_Camera( const _tchar* _pLayerTag, const _vec3 * _pEye, const _vec3 * _pAt, const _vec3 * _pUp, const _float & _fFovY, const _float & _fAspect, const _float & _fNear, const _float & _fFar )
{

	Engine::CGameObject* pObject = CCamera_UseTool::Create( m_pGraphicDev, _pEye, _pAt, _pUp, _fFovY, _fAspect, _fNear, _fFar );
	
	if ( nullptr == pObject )
		return E_FAIL;
	//if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( SCENE_TOOL, _pLayerTag, pObject ) ) )
	//	return E_FAIL;

	if ( FAILED( m_pCamera_Manager->Add_Camera( SCENE_TOOL, _pLayerTag, (CCamera*)pObject ) ) )
		return E_FAIL;

	if ( FAILED( m_pCamera_Manager->Set_CurrentCamera( (CCamera*)pObject ) ) )
		return E_FAIL;

	return NOERROR;

}

HRESULT CScene_Tool::Ready_Layer_Terrain( const _tchar * _pLayerTag )
{
	Engine::CGameObject* pObject = nullptr;
	
	pObject = CTerrain_ToolUse::Create( m_pGraphicDev );

	if ( nullptr == pObject )
		return E_FAIL;
	if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( SCENE_TOOL, _pLayerTag, pObject ) ) )
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Tool::Ready_Layer_Object( const _tchar * _pLayerTag )
{
	Engine::CGameObject* pObject = nullptr;

	OBJECTDATA ObjectData;
	ZeroMemory( &ObjectData, sizeof( OBJECTDATA ) );

	ObjectData.bStaticMesh = false;
	ObjectData.pMeshTag = L"CMesh_Object";
	ObjectData.vPos = _vec3( 10.f, 0.f, 10.f );
	ObjectData.vRot = _vec3( 0.f, 0.f, 0.f );
	ObjectData.vScale = _vec3( 5.f, 5.f, 5.f );

	pObject = CObject_ToolUse::Create( m_pGraphicDev, ObjectData );

	if ( nullptr == pObject )
		return E_FAIL;
	if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( SCENE_TOOL, _pLayerTag, pObject ) ) )
		return E_FAIL;

	return NOERROR;
}

CScene_Tool * CScene_Tool::Create( LPDIRECT3DDEVICE9 _pGraphicDev )
{
	CScene_Tool* pInstance = new CScene_Tool( _pGraphicDev );

	if ( FAILED( pInstance->Ready_Scene() ) )
	{
		CREATE_FAIL( pInstance, "CScene_Tool" );
	}

	return pInstance;
}

_ulong CScene_Tool::Free( void )
{
	CLight_Manager::GetInstance()->Release_Light();

	return Engine::CScene::Free();
}
