#pragma once

#include "Defines.h"
#include "Equipment.h"

namespace Engine
{
	class CRenderer;
	class CTransform;		
	class CMesh_Static;
	class CShader;
}

class CBomb_Mask : public CEquipment
{
private:
	explicit CBomb_Mask(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBomb_Mask() = default;
public:
	
public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
private:
	_float							m_fAngle = 0.f;

private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CBomb_Mask* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

