#pragma once

#include "Engine_Defines.h"
#include "Defines.h"
#include "Base.h"

// 용도 : 객체들을 렌더링 또는 사용함에 있어 필요한 부품객체들을 모아서 관리한다.

class CMonster;
class CCharacterGroupLayer;
class CGroupManager : public CBase
{
	DECLARE_SINGLETON(CGroupManager)
private:
	enum CharacterKind { Character_Mous, Character_KnifeThug_Default, Character_KnifeThug_Knife, Character_SlimThug, Character_BigThug, Character_Soldier, Character_Soldier_Bomb, Character_End };
private:
	explicit CGroupManager( void );
	virtual ~CGroupManager() = default;
public:
#ifndef _FINAL
	HRESULT Add_Monster( const _uint& _iPlayerSceneID, const _tchar* _pPlayerLayerName, CMonster* _pMonster );
#endif
public:
	HRESULT Ready_GroupManager( LPDIRECT3DDEVICE9 pGraphicDevice, const SCENETYPE& _eSceneID );
	void Update_GroupManager( const _float& _fTimeDelta );
public:
	void NextGroupSetting( void );
private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
private:
	CCharacterGroupLayer**			m_ppCharGroupLayer = nullptr;
	_int							m_iCurLayerIndex = -1;
	_uint							m_iLayerCount = 0;
private:
	HRESULT Create_Stage1Group( void );
private:
	HRESULT Create_Character( list<CMonster*>& _MonsterList, const CharacterKind& _eCharKind, const _uint& _iCreateCount );
	HRESULT Add_CharGroup( const list<CMonster*>& _MonsterList );
	HRESULT Ready_GroupLayerData( void* _pCharGroupLayerData );
private:
	virtual _ulong Free( void ) final;
};
