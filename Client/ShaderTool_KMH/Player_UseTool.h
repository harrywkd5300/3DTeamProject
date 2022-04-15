#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CShader;
	class CRenderer;
	class CMesh_Dynamic;
}

class CPlayer_UseTool :
	public Engine::CGameObject
{
public:
	explicit CPlayer_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CPlayer_UseTool() = default;
public:
	HRESULT Ready_GameObject( void );
	_int Update_GameObject( const _float& _fTimeDelta );
	_int LastUpdate_GameObject( const _float& _fTimeDelta );
	void Render_GameObject( void );
private:
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CMesh_Dynamic*		m_pMeshCom = nullptr;
	LPD3DXEFFECT				m_pEffect = nullptr;
private:
	virtual _ulong Free( void ) override;
};

