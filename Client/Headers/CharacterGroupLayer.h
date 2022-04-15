#pragma once
#include "Engine_Defines.h"
#include "Defines.h"
#include "Base.h"

namespace Engine
{
	class CCharacter;
	class CObject_Manager;
}

class CMonster;
class CCharacterGroup;
class CCharacterGroupLayer :
	public CBase
{
public:
	typedef struct tagCharGroupData
	{
		Engine::CCharacter*	pTarget = nullptr;
	} CHARGROUPDATA;

	typedef struct tagCharGroupLayerData
	{
		CHARGROUPDATA*				pCharGroupData = nullptr;
		_uint						iGroupCount = 0;
		list<CMonster*>				CharacterList;
	} CHARGROUPLAYERDATA;
public:
	explicit CCharacterGroupLayer( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CCharacterGroupLayer() = default;
public:
	HRESULT OnActive( const SCENETYPE& _eSceneID, const _tchar* _pLayerTag );
	void DieGroupCharacter( CCharacterGroup* _pGroup );
public:
	HRESULT Add_Character( CMonster* _pMonster );
public:
	void Update_CharacterGroupLayer( const _float& _fTimeDelta );
private:
	LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
private:
	CCharacterGroup**			m_pCharacterGroupArr = nullptr;
	CHARGROUPLAYERDATA			m_CharGroupLayerData;
	CObject_Manager*			m_pObject_Manager = nullptr;
private:
	HRESULT Ready_CharacterGroupLayer( const CHARGROUPLAYERDATA& _CharGroupLayerData );
public:
	static CCharacterGroupLayer* Create( LPDIRECT3DDEVICE9 _pGraphicDev, const CHARGROUPLAYERDATA& _CharGroupLayerData );
private:
	virtual _ulong Free( void ) override;
};

