#pragma once
#include "Tool_Defines.h"
#include "Equipment.h"

namespace Engine
{
	class CRenderer;
	class CTransform;		
	class CMesh_Static;
	class CCollider;
	class CShader;
}

class CSai_UseTool : public CEquipment
{
private:
	explicit CSai_UseTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSai_UseTool() = default;
public:
	
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CCollider*				m_pColliderCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
private:
	_float							m_fAngle = 0.f;
private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSai_UseTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

