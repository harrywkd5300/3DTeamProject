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

class CKarari final : public CMonster
{
private:
	enum { KarariColGroup_Hit, Karari_RWeapon_ColGroup_Attack, Karari_LWeapon_ColGroup_Attack, Karari_RFoot_ColGroup_Attack, Karari_LFoot_ColGroup_Attack, KarariColGroup_End };

	enum eBigThugAtk { Karari_Atk_End };

private:
	explicit CKarari(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKarari() = default;

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
	static CKarari* Create(LPDIRECT3DDEVICE9 pGraphicDev);


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



	// 멤버 변수
private:
	_int	m_iAtk_Count = Karari_Atk_End;
	_int	m_iSelect_Atk_Count = 0;
};

