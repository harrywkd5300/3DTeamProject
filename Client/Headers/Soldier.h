#pragma once

#include "Defines.h"
#include "Monster.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CPicking;
	class CMaterial;
	class CVIBuffer;
	class CMesh_Dynamic;
	class CCollider;
	class CNavigation;
	class CShader;
}

class CSoldier final : public CMonster
{
private:
	enum { SoldierColGroup_Hit, SoldierColGroup_Attack, SoldierColGroup_End };

	enum eBigThugAtk { Soldier_Defalut_Atk, Soldier_Combo_Atk, Soldier_Jump_Atk };

private:
	explicit CSoldier(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSoldier() = default;

public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
	virtual HRESULT Ready_AI(void) final;

	virtual void SignalUpdate(CCharacterGroup::CHARDATA* ptChar) final;
	virtual void Move_SignalUpdate(CCharacterGroup::CHARDATA* ptChar) final;

private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSoldier* Create(LPDIRECT3DDEVICE9 pGraphicDev);


	//--------- Debug �� ���� --------------
private:

	//--------- Debug �� ���� --------------
public:
	virtual EStatus Move_Forward(float fScale = 1.f);
	virtual EStatus Move_Right(float fScale = 1.f);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;



	// ��� ����
private:
	_int	m_iAtk_Count = Soldier_Defalut_Atk;
	_int	m_iSelect_Atk_Count = 0;
};

