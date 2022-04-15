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

class CSlimThug final : public CMonster
{
private:
	enum { SlimThugColGroup_Hit, SlimThugColGroup_Attack, SlimThugColGroup_End };
private:
	explicit CSlimThug(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlimThug() = default;
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

	virtual void SignalUpdate(CCharacterGroup::CHARDATA* ptChar) final;
	virtual void Move_SignalUpdate(CCharacterGroup::CHARDATA* ptChar) final;

private:
	virtual HRESULT Ready_AI(void) final;
	HRESULT Ready_Component( void );
	HRESULT Ready_DefaultSetting( void );
private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSlimThug* Create(LPDIRECT3DDEVICE9 pGraphicDev);
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
};

