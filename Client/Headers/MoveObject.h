#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CCollider;
	class CFrustum;
	class CShader;
}

class CMoveObject final : public CGameObject
{
private:
	explicit CMoveObject(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual ~CMoveObject() = default;
public:
	_float Get_CullSize( void ) const { return m_fCullSize; }
public:
	virtual HRESULT Ready_GameObject(MAPOBJECT pData, _bool Combine, _matrix* pWorld) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CCollider*				m_pColliderCom = nullptr;
	Engine::CFrustum*				m_pFrustumCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;

private:				// 오브젝트 Data
	MAPOBJECT						m_ObjcetData;
private:
	_bool							m_isDraw = true;
	_matrix			matView, matProj,matWV;

	_float			m_fCullSize		= 0.f;

	_float			m_fMoveSpeed	= 0.f;

	// Tunnel
	_float			m_fTunnelSize = 384.f;	// MaxSize]
	_bool			m_bStart = true;
	_bool			m_bEnd	 = false;

	// Subway
	_float			m_fShakePower = 0.f;

	_bool			m_bShake = false;
	_float			m_fShake = 0.f;
	_float			m_fShakeCnt = 0.f;

	_bool			m_bShakeRight = true;
	_bool			m_bShakeEnd = false;


private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Ready_Mesh(const _tchar * pComname);

private:
	void	Move_Tunnel(const _float& fTimeDelta);
	void	Move_Subway(const _float& fTimeDelta);

public:
	static CMoveObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, MAPOBJECT pData, _bool Combine, _matrix* pWorld = nullptr);
public:
	void Debug_Render(void);
public:
	virtual _ulong Free(void);
};

