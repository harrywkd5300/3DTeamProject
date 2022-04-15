#pragma once

#include "Defines.h"
#include "Monster.h"
#include "Bomb.h"

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

class CSoldier_Bomb final : public CMonster
{
private:
	enum { Soldier_BombColGroup_Hit, Soldier_BombColGroup_Attack, Soldier_BombColGroup_End };

	enum eBigThugAtk { Soldier_Bomb_Defalut_Atk, Soldier_Bomb_Combo_Atk, Soldier_Bomb_Jump_Atk, Soldier_Bomb_Throw_Bomb };

private:
	explicit CSoldier_Bomb(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSoldier_Bomb() = default;

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
	
	void Create_Bomb(void);

public:
	static CSoldier_Bomb* Create(LPDIRECT3DDEVICE9 pGraphicDev);


	//--------- Debug 용 변수 --------------
private:

	//--------- Debug 용 변수 --------------
public:
	virtual EStatus Move_Forward(float fScale = 1.f);
	virtual EStatus Move_Right(float fScale = 1.f);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;



private:
	CBomb*		m_pBomb = nullptr;

	// 멤버 변수
private:
	_matrix*			m_pBombHand_mat = nullptr;
	D3DXTRACK_DESC*		m_pTrack = nullptr;

	_bool				m_bThrow_On = false;
	_bool				m_bCreate_Bomb = true;

	_int				m_iAtk_Count = Soldier_Bomb_Defalut_Atk;
	_int				m_iSelect_Atk_Count = 0;

};

