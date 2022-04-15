#include "stdafx.h"
#include "..\Headers\CharacterGroupLayer.h"

#include "Object_Manager.h"

#include "CharacterGroup.h"
#include "Monster.h"

CCharacterGroupLayer::CCharacterGroupLayer( LPDIRECT3DDEVICE9 _pGraphicDev )
	: m_pGraphicDev( _pGraphicDev )
	, m_pObject_Manager( CObject_Manager::GetInstance() )
{
	m_pGraphicDev->AddRef();
	m_pObject_Manager->AddRef();
}

HRESULT CCharacterGroupLayer::OnActive( const SCENETYPE& _eSceneID, const _tchar* _pLayerTag )
{
	for ( auto& Character : m_CharGroupLayerData.CharacterList )
	{
		if ( FAILED( m_pObject_Manager->Ready_GameObject( _eSceneID, _pLayerTag, Character ) ) )
			return E_FAIL;
	}

	return NOERROR;
}

void CCharacterGroupLayer::DieGroupCharacter( CCharacterGroup* _pGroup )
{
	_uint iTemp = 0;
	_uint iMaxCount = 0;
	_int iMaxIndex = -1;

	CDebug::GetInstance()->Add_ConsoleDebugText( L"Die Character" );

	for ( _uint i = 0; i < m_CharGroupLayerData.iGroupCount; ++i )
	{
		// 남아 있는 캐릭터의 개수를 받아온다..
		iTemp = m_pCharacterGroupArr[i]->Get_CharSize();

		if ( iMaxCount < iTemp )	// 가장 많은 캐릭터가 있는 그룹을 찾음..
		{
			iMaxCount = iTemp;
			iMaxIndex = i;
		}
	}

	if ( -1 == iMaxIndex )
		return;
	if ( _pGroup->GetKey() == m_pCharacterGroupArr[iMaxIndex]->GetKey() )
		return;

	_uint iGroupCharCount = _pGroup->Get_CharSize();

	// 개수 차이를 구한다..
	_uint iContract = iMaxCount - iGroupCharCount;

	// 1 개 차이 정도는 무시 ( 2 개 이상 차이가 날 때 부터 다른 그룹의 그룹원을 옮기게 함 )..
	if ( iContract <= 1 )
		return;

	CDebug::GetInstance()->Add_ConsoleDebugText( L"Group Change" );

	iContract -= 1;
	for ( _uint i = 0; i < iContract; ++i )	// 많은 그룹원이 속한 그룹의 그룹원을 옮긴다..
	{
		CMonster* pCharacter = m_pCharacterGroupArr[iMaxIndex]->Erase_Character();
		if ( nullptr == pCharacter )
			return;
		_pGroup->Add_Character( pCharacter );
	}
}

HRESULT CCharacterGroupLayer::Add_Character( CMonster * _pMonster )
{
	m_CharGroupLayerData.CharacterList.push_back( _pMonster );
	m_pCharacterGroupArr[m_CharGroupLayerData.CharacterList.size() % 4]->Add_Character( _pMonster );

	if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( 0, L"Monster", _pMonster ) ) )
		return E_FAIL;

	return NOERROR;
}

void CCharacterGroupLayer::Update_CharacterGroupLayer( const _float & _fTimeDelta )
{
	_uint iTotalCharCount = 0;
	_uint iTemp = 0;
	_uint iMaxCount = 0;

	for ( _uint i = 0; i < m_CharGroupLayerData.iGroupCount; ++i )
	{
		m_pCharacterGroupArr[i]->Update_CharacterGroup( _fTimeDelta );
	}
}

HRESULT CCharacterGroupLayer::Ready_CharacterGroupLayer( const CHARGROUPLAYERDATA& _CharGroupLayerData )
{
	m_CharGroupLayerData = _CharGroupLayerData;

	m_pCharacterGroupArr = new CCharacterGroup*[m_CharGroupLayerData.iGroupCount];

	for ( _uint i = 0; i < m_CharGroupLayerData.iGroupCount; ++i )
	{
		m_pCharacterGroupArr[i] = CCharacterGroup::Create( m_pGraphicDev, m_CharGroupLayerData.pCharGroupData[i].pTarget, this );

		if ( nullptr == m_pCharacterGroupArr[i] )
			return E_FAIL;
	}

	_uint iIndex = 0;
	for ( auto& Character : m_CharGroupLayerData.CharacterList )
	{
		if ( Character == nullptr )
			return E_FAIL;

		m_pCharacterGroupArr[iIndex]->Add_Character( Character );

		iIndex = (iIndex + 1) % m_CharGroupLayerData.iGroupCount;
	}

	return NOERROR;
}

CCharacterGroupLayer * CCharacterGroupLayer::Create( LPDIRECT3DDEVICE9 _pGraphicDev, const CHARGROUPLAYERDATA & _CharGroupLayerData )
{
	CCharacterGroupLayer* pInstance = new CCharacterGroupLayer( _pGraphicDev );

	if ( FAILED( pInstance->Ready_CharacterGroupLayer( _CharGroupLayerData ) ) )
	{
		CREATE_FAIL( pInstance, "CCharacterGroupLayer" );
	}

	return pInstance;
}

_ulong CCharacterGroupLayer::Free( void )
{
	_ulong dwRefCnt = 0;

	for ( _uint i = 0; i < m_CharGroupLayerData.iGroupCount; ++i )
	{
		dwRefCnt = Engine::Safe_Release( m_pCharacterGroupArr[i] );
		dwRefCnt = Engine::Safe_Release( m_CharGroupLayerData.pCharGroupData[i].pTarget );
	}
	Engine::Safe_Delete_Array( m_pCharacterGroupArr );
	Engine::Safe_Delete_Array( m_CharGroupLayerData.pCharGroupData );

	m_CharGroupLayerData.CharacterList.clear();

	dwRefCnt = Engine::Safe_Release( m_pObject_Manager );
	dwRefCnt = Engine::Safe_Release( m_pGraphicDev );

	return 0;
}
