#pragma once
#include "Engine_Defines.h"
#include "Defines.h"
#include "Base.h"

namespace Engine
{
	class CCharacter;
	class CController;
	class CTransform;
};

class CMonster;
class CCharacterGroupLayer;
class CCharacterGroup final :
	public CBase
{
public:
	struct tagCharData;

	typedef struct tagGroupData
	{
		list<tagCharData>	CharList;
		_float				fMaxSpeed = 4.f;
		_float				fMaxForce = 0.3f;
		_uint				iCharCount = 0;		// 그룹의 속해 있는 캐릭터들의 개수..
	} GROUPDATA;

	typedef struct tagCharData
	{
		CMonster*				pCharacter = nullptr;
		Engine::CTransform*		pTransform = nullptr;
		Engine::CController*	pController = nullptr;
		_vec3					vPos = _vec3( 0.f, 0.f, 0.f );
		_vec3					vVelocity = _vec3( 0.f, 0.f, 0.f );
		_float					fTime_Atk = 8.f;
		_float					fTime = 0.5f;
		_ubyte					iActState = 0;

		void Release( void );

		void Separate( _vec3& _vOut, const GROUPDATA& _GroupData );
		void Align( _vec3 & _vOut, const GROUPDATA & _GroupData );
		void Cohesion( _vec3 & _vOut, const GROUPDATA & _GroupData );
		void Limit( _vec3& _vOut, const _float& _fMax );
	} CHARDATA;
public:
	explicit CCharacterGroup( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CCharacterGroup() = default;
public:
	_uint Get_CharSize( void );
public:
	HRESULT Add_Character( CMonster* _pCharacter );
	CMonster* Erase_Character( void );
public:
	void Update_CharacterGroup( const _float& _fTimeDelta );
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	CCharacterGroupLayer*	m_pCharGroupLayer = nullptr;
private:
	Engine::CCharacter*			m_pTarget = nullptr;	// 그룹의 적..
	GROUPDATA					m_GroupData;			// 그룹의 정보..
	_uint						m_iWaitFrame = 0;
private:
	HRESULT Ready_CharacterGroup( Engine::CCharacter* _pTarget, CCharacterGroupLayer* _pCharGroupLayer );
public:
	static CCharacterGroup* Create( LPDIRECT3DDEVICE9 _pGraphicDev, Engine::CCharacter* _pTarget, CCharacterGroupLayer* _pCharGroupLayer );
private:
	virtual _ulong Free( void ) override;
};

