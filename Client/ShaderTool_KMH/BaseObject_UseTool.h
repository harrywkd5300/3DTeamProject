#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CCollider;
	class CFrustum;
	class CShader;
	class CPicking;
}

class CBaseObject_UseTool final : public CGameObject
{
private:
	explicit CBaseObject_UseTool(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual ~CBaseObject_UseTool() = default;
public:
	_float Get_CullSize( void ) const { return m_fCullSize; }
public:
	virtual HRESULT Ready_GameObject(MAPOBJECT pData, _bool Combine, _matrix* pWorld) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
public:
	_bool PickingCheck( _vec3* _pOut, Engine::CPicking* _pPickingCom );
private:
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CFrustum*				m_pFrustumCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
private:				// ������Ʈ Data
	MAPOBJECT						m_ObjcetData;
private:
	_bool							m_isDraw = true;
	_matrix			matView, matProj,matWV;

	_float			m_fCullSize		= 0.f;
private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Ready_Mesh(const _tchar * pComname);

public:
	static CBaseObject_UseTool* Create(LPDIRECT3DDEVICE9 pGraphicDev, MAPOBJECT pData, _bool Combine, _matrix* pWorld = nullptr);
public:
	void Debug_Render(void);
public:
	//void Debug_Render();
	virtual _ulong Free(void);
};

