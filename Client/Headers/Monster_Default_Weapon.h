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

class CMonster_Default_Weapon : public CEquipment
{
public:
	enum eMonster_Weapon { Bat, SpikeBat, Pipe, PipeWRench, Monster_Weapon_End };

private:
	explicit CMonster_Default_Weapon(LPDIRECT3DDEVICE9 pGraphicDev, eMonster_Weapon eWeapon);
	virtual ~CMonster_Default_Weapon() = default;
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

	eMonster_Weapon					m_eWeapon = Monster_Weapon_End;

private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMonster_Default_Weapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, eMonster_Weapon eWeapon);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

