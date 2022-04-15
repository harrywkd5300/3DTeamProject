#include "..\Headers\Collider_Manager.h"
#include "Collider.h"

USING(Engine)
IMPLEMENT_SINGLETON( CCollider_Manager )

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����ü�� �Լ� ���Ǻ�..
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_bool CCollider_Manager::tagColliderGroup::IsInCollisionList( const _ulong & _dwColliderKey )
{
	for ( auto& CollisionCollider : CollisionList )
	{
		if ( CollisionCollider->GetKey() == _dwColliderKey )
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Add_Collider( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, CCollider* _pCollider, Engine::CGameObject* _pGameObject,
										 std::function<void( const Engine::Collision& )> _pEventFunction )
{
	if ( nullptr == _pCollider ||
		 _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return E_FAIL;

	if ( nullptr != _pGameObject )
		_pCollider->Set_GameObject( _pGameObject );
	_pCollider->Set_GroupName( _pLayerTag );

	COLLIDERGROUP* pFindGroup = Find_ColliderGroup( _pLayerTag, _eGroupKind );
	COLLIDERDATA ColliderData;
	ColliderData.pCollider = _pCollider;
	_pCollider->AddRef();

	if ( nullptr == pFindGroup )
	{
		COLLIDERGROUP ColliderGroup( ColliderData, _pLayerTag );
		m_ColliderGroupList[_eGroupKind].push_back( ColliderGroup );
	}
	else
		pFindGroup->AddCollider( ColliderData );

	return NOERROR;
}

HRESULT CCollider_Manager::Add_ColliderGroup( const GROUPKIND & _eGroupKind, const _tchar * _pLayerTag, const _tchar * _pFindGroupTag )
{
	if ( _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return E_FAIL;

	COLLIDERLAYER* pFindLayer = Find_ColliderLayer( _pLayerTag );
	COLLIDERGROUP* pFindGroup = Find_ColliderGroup( _pFindGroupTag, GROUP_STATIC );
	if ( nullptr == pFindGroup )
	{
		pFindGroup = Find_ColliderGroup( _pFindGroupTag, GROUP_DYNAMIC );
		if ( nullptr == pFindGroup )
			return E_FAIL;
	}

	if ( nullptr == pFindLayer )
	{
		COLLIDERLAYER ColliderLayer( pFindGroup, _pLayerTag, m_ColliderLayerList[_eGroupKind].size() );
		m_ColliderLayerList[_eGroupKind].push_back( ColliderLayer );
	}
	else
		pFindLayer->ColliderGroupList.push_back( pFindGroup );

	return NOERROR;
}

HRESULT CCollider_Manager::Activate_ColliderGroup( const GROUPKIND & _eGroupKind, const _tchar * _pLayerTag, const _tchar * _pFindGroupTag )
{
	if ( _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return E_FAIL;

	return NOERROR;
}

_bool CCollider_Manager::Delete_Collider( const _tchar * _pLayerTag, Engine::CCollider* _pCollider )
{
	COLLIDERGROUP* pFindGroup = Find_ColliderGroup( _pLayerTag, GROUP_STATIC );
	if ( nullptr == pFindGroup )
	{
		if ( !(pFindGroup = Find_ColliderGroup( _pLayerTag, GROUP_DYNAMIC )) )
			return false;
	}

	_ulong dwFindKey = _pCollider->GetKey();
	for ( _uint i = 0; i < pFindGroup->iCollisionDataCount; ++i )
	{
		if ( pFindGroup->pColliderData[i].pCollider->GetKey() == dwFindKey )
		{
			Engine::Safe_Release( pFindGroup->pColliderData[i].pCollider );
			for ( _uint j = i; j < pFindGroup->iCollisionDataCount - 1; ++j )
			{
				pFindGroup->pColliderData[j].pCollider = pFindGroup->pColliderData[j + 1].pCollider;
				pFindGroup->pColliderData[j].pEventFunction = pFindGroup->pColliderData[j + 1].pEventFunction;
			}

			--pFindGroup->iCollisionDataCount;

			if ( pFindGroup->iCollisionDataCount == 0 )
				Engine::Safe_Delete_Array( pFindGroup->pColliderData );

			return true;
		}
	}

	return false;
}

_bool CCollider_Manager::Delete_ColliderInGroup( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag )
{
	if ( _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return false;

	for ( auto iter = m_ColliderGroupList[_eGroupKind].begin(); iter != m_ColliderGroupList[_eGroupKind].end(); ++iter )
	{
		if ( 0 == lstrcmp( iter->pGroupName, _pFindGroupTag ) )
		{
			while ( iter->iCollisionDataCount > 0 )
			{
				if ( false == Delete_Collider( _pFindGroupTag, iter->pColliderData[0].pCollider ) )
					return false;
			}

			m_ColliderGroupList[_eGroupKind].erase( iter );

			return true;
		}
	}

	return false;
}

_bool CCollider_Manager::Delete_ColliderLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindLayerTag )
{
	if ( _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return false;

	for ( auto iter = m_ColliderLayerList[_eGroupKind].begin(); iter != m_ColliderLayerList[_eGroupKind].end(); ++iter )
	{
		if ( 0 == lstrcmp( iter->pLayerName, _pFindLayerTag ) )
		{
			iter = m_ColliderLayerList[_eGroupKind].erase( iter );

			for ( ; iter != m_ColliderLayerList[_eGroupKind].end(); ++iter )
				--iter->dwPriorityKey;

			return true;
		}
	}

	return false;
}

_bool CCollider_Manager::Exclude_ColliderGroupInLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag, const _tchar* _pFindLayerTag )
{
	if ( _eGroupKind >= GROUP_END ||
		 _eGroupKind <= GROUP_NONE )
		return false;

	for ( auto& Layer : m_ColliderLayerList[_eGroupKind] )
	{
		if ( 0 == lstrcmp( Layer.pLayerName, _pFindLayerTag ) )
		{
			for ( auto iter = Layer.ColliderGroupList.begin(); iter != Layer.ColliderGroupList.end(); ++iter )
			{
				if ( 0 == lstrcmp( (*iter)->pGroupName, _pFindGroupTag ) )
				{
					Layer.ColliderGroupList.erase( iter );
					return true;
				}
			}

			return false;
		}
	}

	return false;
}

HRESULT CCollider_Manager::Ignore_ColliderLayer( const _tchar * _pLayerTag, const _tchar * _pIgnoreLayerTag )
{
	COLLIDERLAYER* pFindLayer = Find_ColliderLayer( _pLayerTag );
	COLLIDERLAYER* pIgnoreLayer = Find_ColliderLayer( _pIgnoreLayerTag );

	if ( nullptr == pFindLayer || nullptr == pIgnoreLayer )
		return E_FAIL;

	for ( auto& IgnoreLayer : pFindLayer->IgnoreLayerList )
	{
		if ( IgnoreLayer->dwPriorityKey == pIgnoreLayer->dwPriorityKey )	// �̹� ������ ���̾� ����Ʈ�� �ִٸ� ����..
			return NOERROR;
	}

	pFindLayer->IgnoreLayerList.push_back( pIgnoreLayer );

	return NOERROR;
}

HRESULT CCollider_Manager::Ready_Collider_Manager( void )
{
	return NOERROR;
}

void CCollider_Manager::Update_Collider_Manager( void )
{
#ifndef _FINAL
	_float asdasd = 0.f;
	_bool bErrasd = false;

	CHECK_TIME_START
#endif
	{
		for ( _ubyte i = 0; i < GROUP_END; ++i )
		{
			if ( m_ColliderGroupList[i].empty() )
				continue;

			auto iterEnd = m_ColliderLayerList[i].end();

			for ( auto& DstIter : m_ColliderLayerList[i] )
			{
				// �ݸ��� ���̾�鳢�� �˻��� ���� �� �˻��� �ʿ� ���� ������
				// 1�� ���̾�� 2�� ���̾� �浹 �˻��ϴ� �� == 2�� ���̾�� 1�� ���̾�� �浹 �˻��ϴ� ��..
				auto SrcIter = m_ColliderLayerList[i].begin();
				while ( DstIter.dwPriorityKey != SrcIter->dwPriorityKey )
					++SrcIter;

				for ( ++SrcIter; SrcIter != iterEnd; ++SrcIter )
				{			
					_bool bContinue = false;

					// �����ؾ� �ϴ� �ݸ��� ���̾ �ɷ����� �۾�..
					if ( bContinue = DstIter.IgnoreCheck( *SrcIter ) )
						continue;
					else if ( bContinue = SrcIter->IgnoreCheck( DstIter ) )
						continue;

					// �׷� ���� �浹 �� �۾�..
					for ( auto& DstColliderGroup : DstIter.ColliderGroupList )
					{
						for ( auto& SrcColliderGroup : SrcIter->ColliderGroupList )
						{
							CollisionGroupToGroup( DstColliderGroup, SrcColliderGroup );
						}
					}
				}

				// �ݸ��� �׷��� �浹 ����Ʈ�� ����..
				for ( auto& DstColliderGroup : DstIter.ColliderGroupList )
				{
					DstColliderGroup->CollisionList.clear();
					DstColliderGroup->CollisionList = DstColliderGroup->CurCollisionList;
					DstColliderGroup->CurCollisionList.clear();
				}
			}
		}
	}
#ifndef _FINAL
	CHECK_TIME_END( asdasd, bErrasd );
	CDebug::GetInstance()->Add_ColliderRunTime(asdasd);
	//if ( bErrasd )
	//{
	//	CDebug::GetInstance()->Add_ConsoleDebugText( asdasd );
	//}
#endif

}

void CCollider_Manager::Release_DynamicLayer( void )
{
	Release_Group( GROUP_DYNAMIC );

	m_ColliderLayerList[GROUP_DYNAMIC].clear();
}

void CCollider_Manager::CollisionGroupToGroup( COLLIDERGROUP* _pDstColliderGroup, COLLIDERGROUP* _pSrcColliderGroup )
{
	COLLIDERDATA* pDstColliderData = nullptr;
	COLLIDERDATA* pSrcColliderData = nullptr;

	COLLISIONDATA CollisionData;

	for ( _uint i = 0; i < _pDstColliderGroup->iCollisionDataCount; ++i )
	{
		pDstColliderData = &_pDstColliderGroup->pColliderData[i];
		for ( _uint j = 0; j < _pSrcColliderGroup->iCollisionDataCount; ++j )
		{
			ZeroMemory( &CollisionData, sizeof( COLLISIONDATA ) );
			pSrcColliderData = &_pSrcColliderGroup->pColliderData[j];

			// ���� �浹 �˻� ( true == �浹�Ǿ��� )..
			if ( true == pDstColliderData->pCollider->Collision_Sphere( pSrcColliderData->pCollider, CollisionData ) )
			{
				// --------------------------- �浹 Ÿ�� �����ϴ� �κ�.. --------------------------- //
				COLLISIONTYPE eCollisionType = COLLISION_ENTER;
				
				_ulong dwFindKey = pSrcColliderData->pCollider->GetKey();
				if ( true == _pDstColliderGroup->IsInCollisionList( dwFindKey ) )	// �浹 �� ���� �� �浹��..
				{
					eCollisionType = COLLISION_STAY;
				}
				
				_pDstColliderGroup->CurCollisionList.push_back( pSrcColliderData->pCollider );
				_pSrcColliderGroup->CurCollisionList.push_back( pDstColliderData->pCollider );
				
				// �浹 �Ǿ��ٸ� �浹 �ߴٴ� �޽����� �浹�� �ݶ��̴��鿡�� ����..
				OnCollisionEvent( pDstColliderData, pSrcColliderData, eCollisionType, CollisionData );

				return;	// �׷� �� �׷� ���� �浹�� �� �� �浹�ϸ� �� ������ �� �˻��ϵ���..
			}
		}
	}
}

void CCollider_Manager::OnCollisionEvent( COLLIDERDATA* _pDstColliderData, COLLIDERDATA* _pSrcColliderData, const COLLISIONTYPE& _eCollisionType,
										  const COLLISIONDATA& _CollisionData )
{
	Collision CollisionData = Collision( _CollisionData, _pDstColliderData->pCollider, _pSrcColliderData->pCollider, _eCollisionType );

	_pDstColliderData->pCollider->OnCollisionEvent( _pDstColliderData->pEventFunction, CollisionData );


	CollisionData.pCollider = _pSrcColliderData->pCollider;
	CollisionData.pOtherCollider = _pDstColliderData->pCollider;

	_pSrcColliderData->pCollider->OnCollisionEvent( _pSrcColliderData->pEventFunction, CollisionData );
}

CCollider_Manager::COLLIDERGROUP* CCollider_Manager::Find_ColliderGroup( const _tchar* _pLayerTag, const GROUPKIND& _eGroupKind )
{
	for ( auto& ColliderGroup : m_ColliderGroupList[_eGroupKind] )
	{
		if ( 0 == lstrcmp( ColliderGroup.pGroupName, _pLayerTag ) )
			return &ColliderGroup;
	}

	return nullptr;
}

CCollider_Manager::COLLIDERLAYER* CCollider_Manager::Find_ColliderLayer( const _tchar * _pLayerTag )
{
	for ( _uint i = 0; i < GROUP_END; ++i )
	{
		for ( auto& ColliderGroup : m_ColliderLayerList[i] )
		{
			if ( 0 == lstrcmp( ColliderGroup.pLayerName, _pLayerTag ) )
				return &ColliderGroup;
		}
	}

	return nullptr;
}

void CCollider_Manager::Release_Group( const GROUPKIND & _eGroupKind )
{
	for ( auto& ColliderGroup : m_ColliderGroupList[_eGroupKind] )
	{
		for ( _uint i = 0; i < ColliderGroup.iCollisionDataCount; ++i )
		{
			Engine::Safe_Release( ColliderGroup.pColliderData[i].pCollider );
		}
	}

	m_ColliderGroupList[_eGroupKind].clear();

	m_ColliderLayerList[_eGroupKind].clear();
}

_ulong CCollider_Manager::Free( void )
{
	for ( _uint i = 0; i < GROUP_END; ++i )
	{
		Release_Group( GROUPKIND( i ) );
	}

	return 0;
}
