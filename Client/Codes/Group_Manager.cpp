#include "stdafx.h"

#include "Group_Manager.h"
#include "Object_Manager.h"
#include "Character.h"

#include "CharacterGroupLayer.h"

#include "Mous.h"
#include "KnifeThug.h"
#include "SlimThug.h"
#include "BigThug.h"
#include "Soldier.h"
#include "Soldier_Bomb.h"

IMPLEMENT_SINGLETON(CGroupManager)

CGroupManager::CGroupManager()
{
}

#ifndef _FINAL
HRESULT CGroupManager::Add_Monster( const _uint& _iPlayerSceneID, const _tchar* _pPlayerLayerName, CMonster* _pMonster )
{
	if ( 0 == m_iLayerCount )
	{
		CCharacterGroupLayer::CHARGROUPLAYERDATA	CharGroupLayerData;

		_uint iCount = CObject_Manager::GetInstance()->Get_NumObject( _iPlayerSceneID, _pPlayerLayerName );

		// 그룹 레이어에 들어갈 데이터 세팅..
		CharGroupLayerData.pCharGroupData = new CCharacterGroupLayer::CHARGROUPDATA[iCount];
		CharGroupLayerData.iGroupCount = iCount;

		for ( _uint i = 0; i < iCount; ++i )
		{
			CharGroupLayerData.pCharGroupData[i].pTarget = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject( _iPlayerSceneID, _pPlayerLayerName, i ));

			if ( nullptr == CharGroupLayerData.pCharGroupData[i].pTarget )
				return E_FAIL;

			CharGroupLayerData.pCharGroupData[i].pTarget->AddRef();
		}


		// 몬스터들 생성..
		CMonster*	pCharacter = _pMonster;

		CharGroupLayerData.CharacterList.push_back( pCharacter );

		// 캐릭터 그룹 레이어 생성 및 값 넘겨서 초기화 시킴..
		m_iLayerCount = 1;
		m_ppCharGroupLayer = new CCharacterGroupLayer*[m_iLayerCount];
		m_ppCharGroupLayer[0] = CCharacterGroupLayer::Create( m_pGraphicDev, CharGroupLayerData );

		// 테스트..
		NextGroupSetting();
	}
	else
	{
		if ( FAILED( m_ppCharGroupLayer[0]->Add_Character( _pMonster ) ) )
			return E_FAIL;
	}

	return NOERROR;
}
#endif

HRESULT CGroupManager::Ready_GroupManager( LPDIRECT3DDEVICE9 pGraphicDevice, const SCENETYPE& _eSceneID )
{
	if ( nullptr == m_pGraphicDev )
	{
		if (pGraphicDevice == nullptr)
			return E_FAIL;

		m_pGraphicDev = pGraphicDevice;
		m_pGraphicDev->AddRef();
	}

	switch ( _eSceneID )
	{
		case SCENE_STAGE:
		{
			if ( FAILED( Create_Stage1Group() ) )
				return E_FAIL;
		}
		break;
	}

	return NOERROR;
}

void CGroupManager::Update_GroupManager( const _float & _fTimeDelta )
{
	if ( m_iCurLayerIndex == -1 ||
		 m_iCurLayerIndex >= m_iLayerCount )
		return;

	m_ppCharGroupLayer[m_iCurLayerIndex]->Update_CharacterGroupLayer( _fTimeDelta );
}

void CGroupManager::NextGroupSetting( void )
{
	++m_iCurLayerIndex;

	if ( m_iCurLayerIndex <= m_iLayerCount )
		m_ppCharGroupLayer[m_iCurLayerIndex]->OnActive( SCENE_STAGE, L"Monster" );
}

HRESULT CGroupManager::Create_Stage1Group( void )
{	
	// 몬스터들 생성..
	list<CMonster*> MonsterList;
	CMonster*	pCharacter = nullptr;
	
	////////////////////////////////////////////////////////////////////////////
	// Create_Character 함수를 사용하여 몬스터를 생성할 수 있도록 수정함..
	// HRESULT Create_Character( 생성할 캐릭터를 담을 리스트, 생성할 캐릭터 타입, 생성할 캐릭터 개수 )
	////////////////////////////////////////////////////////////////////////////

	///* Mous 몬스터 생성.. */
	//{
	//	if ( FAILED( Create_Character( MonsterList, Character_Mous, 1 ) ) )
	//		return E_FAIL;
	//}

	/* Knife Thug 몬스터 생성.. */
	/*{
		if ( FAILED( Create_Character( MonsterList, Character_KnifeThug_Knife, 13 ) ) )
			return E_FAIL;
	}*/
	/*{
		if (FAILED(Create_Character(MonsterList, Character_KnifeThug_Default, 1)))
			return E_FAIL;
	}*/


	/* Slim Thug 몬스터 생성.. */
	{
		if ( FAILED( Create_Character( MonsterList, Character_SlimThug, 4 ) ) )
			return E_FAIL;
	}

	///* Big Thug 몬스터 생성.. */
	//{
	//	if ( FAILED( Create_Character( MonsterList, Character_BigThug, 1 ) ) )
	//		return E_FAIL;
	//}
	
	///* Solider 몬스터 생성.. */
	//{
	//	if ( FAILED( Create_Character( MonsterList, Character_Soldier, 1 ) ) )
	//		return E_FAIL;
	//}

	///* Solider_Bomb 몬스터 생성.. */
	//{
	//	if (FAILED(Create_Character(MonsterList, Character_Soldier_Bomb, 4 )))
	//		return E_FAIL;
	//}

	// 캐릭터 그룹 레이어 생성 및 값 넘겨서 초기화 시킴..
	if ( FAILED( Add_CharGroup( MonsterList ) ) )
		return E_FAIL;
	
	// 테스트..
	NextGroupSetting();

	return NOERROR;
}

HRESULT CGroupManager::Create_Character( list<CMonster*>& _MonsterList, const CharacterKind& _eCharKind, const _uint& _iCreateCount )
{
	CMonster* pCharacter = nullptr;

	for ( _uint i = 0; i < _iCreateCount; ++i )
	{
		switch ( _eCharKind )
		{
			case CGroupManager::Character_Mous:
				pCharacter = CMous::Create( m_pGraphicDev );
				break;
			case CGroupManager::Character_KnifeThug_Default:
				pCharacter = CKnifeThug::Create( m_pGraphicDev, CKnifeThug::Type_Default );
				break;
			case CGroupManager::Character_KnifeThug_Knife:
				pCharacter = CKnifeThug::Create( m_pGraphicDev, CKnifeThug::Type_Knife );
				break;
			case CGroupManager::Character_SlimThug:
				pCharacter = CSlimThug::Create( m_pGraphicDev );
				break;
			case CGroupManager::Character_BigThug:
				pCharacter = CBigThug::Create( m_pGraphicDev );
				break;
			case CGroupManager::Character_Soldier:
				pCharacter = CSoldier::Create(m_pGraphicDev);
				break;
			case CGroupManager::Character_Soldier_Bomb:
				pCharacter = CSoldier_Bomb::Create(m_pGraphicDev);
				break;
			default:
				return E_FAIL;
		}
		
		if ( nullptr == pCharacter )
		{
			for ( auto& Monster : _MonsterList )
				Engine::Safe_Release( Monster );
		}

		_MonsterList.push_back( pCharacter );
	}

	return NOERROR;
}

HRESULT CGroupManager::Add_CharGroup( const list<CMonster*>& _MonsterList )
{
	CCharacterGroupLayer::CHARGROUPLAYERDATA	CharGroupLayerData;

	// 그룹 레이어에 들어갈 데이터 세팅..
	if ( FAILED( Ready_GroupLayerData( &CharGroupLayerData ) ) )
		return E_FAIL;

	CharGroupLayerData.CharacterList = _MonsterList;

	CCharacterGroupLayer** pTemp = new CCharacterGroupLayer*[m_iLayerCount + 1];

	for ( _uint i = 0; i < m_iLayerCount; ++i )
	{
		pTemp[i] = m_ppCharGroupLayer[i];
	}

	if ( 0 < m_iLayerCount )
		Engine::Safe_Delete_Array( m_ppCharGroupLayer );

	pTemp[m_iLayerCount] = CCharacterGroupLayer::Create( m_pGraphicDev, CharGroupLayerData );

	m_ppCharGroupLayer = pTemp;

	if ( nullptr == pTemp[m_iLayerCount] )
		return E_FAIL;

	++m_iLayerCount;

	return NOERROR;
}

HRESULT CGroupManager::Ready_GroupLayerData( void* _pCharGroupLayerData )
{
	CCharacterGroupLayer::CHARGROUPLAYERDATA* pCharGroupLayerData = ((CCharacterGroupLayer::CHARGROUPLAYERDATA*)_pCharGroupLayerData);

	_uint iCount = CObject_Manager::GetInstance()->Get_NumObject( SCENE_STATIC, L"Player" );

	// 그룹 레이어에 들어갈 데이터 세팅..
	pCharGroupLayerData->pCharGroupData = new CCharacterGroupLayer::CHARGROUPDATA[iCount];
	pCharGroupLayerData->iGroupCount = iCount;

	for ( _uint i = 0; i < iCount; ++i )
	{
		pCharGroupLayerData->pCharGroupData[i].pTarget = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject( SCENE_STATIC, L"Player", i ));

		if ( nullptr == pCharGroupLayerData->pCharGroupData[i].pTarget )
			return E_FAIL;

		pCharGroupLayerData->pCharGroupData[i].pTarget->AddRef();
	}

	return NOERROR;
}

_ulong CGroupManager::Free(void)	//By.Jang
{
	_ulong dwRef = 0;

	for ( _uint i = 0; i < m_iLayerCount; ++i )
	{
		Engine::Safe_Release( m_ppCharGroupLayer[i] );
	}
	Engine::Safe_Delete_Array( m_ppCharGroupLayer );

	Safe_Release(m_pGraphicDev);

	return dwRef;
}
