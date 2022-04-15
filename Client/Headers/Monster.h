#pragma once

#include "Defines.h"
#include "Character.h"
#include "CharacterGroup.h"

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
	//class CCharacter;
}

class CMonster : public CCharacter
{
public:
	enum MonsterType { MonsterType_Default, MonsterType_MidBoss,  MonsterType_Boss, MonsterType_End };

protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster() = default;


public:
	virtual HRESULT Ready_GameObject(void) PURE;
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void) PURE;
	virtual HRESULT Ready_AI(void) PURE;
	virtual void SignalUpdate(CCharacterGroup::CHARDATA* ptChar);
public:
	// 충돌 시 호출되는 함수들..
	virtual void OnCollision( const Engine::Collision& _Collision ) override;
public:
	virtual void Move_SignalUpdate(CCharacterGroup::CHARDATA* ptChar);

protected:
	MonsterType		m_eMonsterType = MonsterType_End;

	_float	m_fTempTimeDelta = 0.f;
	_float	m_fEvade_TempTimeDelta = 0.f;

	_int	m_iShieldMax = 0;

	_int	m_iAdd_Shield_Point = 0;
	_float	m_fAdd_Evade_Point = 0.f;

protected:
	HRESULT Ready_Monster_Component();
	void Move_OnTerrain(void);
private:
	HRESULT Ready_Component(void);
	HRESULT Ready_Controller(void);
	
#ifndef _FINAL
	enum { SPHERESIZE = 5 };
	LPD3DXMESH	m_pSphere = nullptr;
	LPDIRECT3DTEXTURE9	m_pGreenTexture = nullptr;
	LPDIRECT3DTEXTURE9	m_pRedTexture = nullptr;
	static _bool m_bFirstMonster;
	static _uint m_iType;
	_matrix		m_matSphereWorld[SPHERESIZE];
protected:
	void Render_DebugSphere( void );
#endif
	//--------- Debug 용 변수 --------------
protected:

	//--------- Debug 용 변수 --------------
public:
	virtual EStatus Move_Forward(float fScale = 1.f);
	virtual EStatus Move_Right(float fScale = 1.f);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void);
};

