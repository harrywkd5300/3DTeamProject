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

class CSoldier_Sword : public CEquipment
{
private:
	explicit CSoldier_Sword(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSoldier_Sword() = default;
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
	static CSoldier_Sword* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

