#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

typedef struct tagObjectData
{
	_vec3 vPos = _vec3( 0.f, 0.f, 0.f );
	_vec3 vRot = _vec3( 0.f, 0.f, 0.f );
	_vec3 vScale = _vec3( 1.f, 1.f, 1.f );
	const _tchar* pMeshTag = nullptr;
	_bool bStaticMesh = true;
} OBJECTDATA;

namespace Engine
{
	class CRenderer;
	class CTexture;
	class CTransform;
	class CShader;
	class CMesh_Static;
	class CMesh_Dynamic;
}

class CObject_ToolUse :
	public Engine::CGameObject
{
private:
	explicit CObject_ToolUse( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CObject_ToolUse() = default;
public:
	HRESULT Ready_Object( const OBJECTDATA& _ObjectData );
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
public:
	static CObject_ToolUse* Create( LPDIRECT3DDEVICE9 _pGraphicDev, const OBJECTDATA& _ObjectData );
private:
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CTransform*		m_pTransformCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;
	Engine::CMesh_Static*	m_pStaticMeshCom = nullptr;
	Engine::CMesh_Dynamic*	m_pDynamicMeshCom = nullptr;
private:
	LPD3DXEFFECT			m_pEffect = nullptr;
private:
	_bool					m_bisStaticMesh = false;
	_float					m_fTimeDelta = 0.f;
private:
	HRESULT Ready_Component( const OBJECTDATA& _ObjectData );
	HRESULT Ready_DefaultSetting( const OBJECTDATA& _ObjectData );
private:
	HRESULT SetUp_OnConstantTable( LPD3DXEFFECT _pEffect );
private:
	virtual _ulong Free( void ) override;

};

